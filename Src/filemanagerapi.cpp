/*!
 Linphone Web - Web plugin of Linphone an audio/video SIP phone
 Copyright (C) 2012-2013 Belledonne Communications
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 
 
 Authors:
 - Yann Diorcet <diorcet.yann@gmail.com>
 
 */

#include "filemanagerapi.h"
#include "coreplugin.h"

#include <global/config.h>
#include <algorithm>

#include <DOM/Window.h>
#include <URI.h>
#include <variant_list.h>
#include <SystemHelpers.h>
#include <BrowserStreamRequest.h>
#include <SimpleStreamHelper.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <boost/bind/protect.hpp>
#include <boost/function.hpp>

#include "factoryapi.h"


/*
 * Boost extension
 */

namespace  boost {
	namespace  filesystem {
		boost::filesystem::path normalize(boost::filesystem::path const &path) {
			boost::filesystem::path absPath = absolute(path);
			boost::filesystem::path::iterator it = absPath.begin();
			boost::filesystem::path result = *it++;
			
			// Get canonical version of the existing part
			for (; exists(result / *it) && it != absPath.end(); ++it) {
				result /= *it;
			}
			result = canonical(result);
			
			// For the rest remove ".." and "." in a path with no symlinks
			for (; it != absPath.end(); ++it) {
				// Just move back on ../
				if (*it == "..") {
					result = result.parent_path();
				}
				// Ignore "."
				else if (*it != ".") {
					// Just cat other path entries
					result /= *it;
				}
			}
			
			return boost::filesystem::path(result.generic_string());
		}
	}
}

namespace LinphoneWeb {

/*
 * Protocol
 */

const std::string FileManagerAPI::Protocol::http("http");
const std::string FileManagerAPI::Protocol::https("https");
const std::string FileManagerAPI::Protocol::internal("internal");
const std::string FileManagerAPI::Protocol::temp("tmp");
const std::string FileManagerAPI::Protocol::local("local");

FileManagerAPI::Protocol::Protocol(std::string const &protocol, boost::filesystem::path const &path): mProtocol(protocol), mPath(path) {
}

std::string const &FileManagerAPI::Protocol::getProtocol() {
	return mProtocol;
}

boost::filesystem::path const &FileManagerAPI::Protocol::getPath() {
	return mPath;
}


/*
 * FileManagerAPI
 */

FileManagerAPI::FileManagerAPI():
	WrapperAPI(APIDescription(this)) {
	FBLOG_DEBUG("FileManagerAPI::FileManagerAPI", "this=" << this);
}

void FileManagerAPI::initProxy() {
	registerMethod("copy", make_method(this, &FileManagerAPI::copy));
	registerMethod("exists", make_method(this, &FileManagerAPI::exists));
	registerMethod("mkdir", make_method(this, &FileManagerAPI::mkdir));
	registerMethod("remove", make_method(this, &FileManagerAPI::remove));
}
	
FileManagerAPI::~FileManagerAPI() {
	FBLOG_DEBUG("FileManagerAPI::~FileManagerAPI", "this=" << this);
}

void FileManagerAPI::setFactory(FactoryAPIPtr factory) {
	WrapperAPI::setFactory(factory);
	initializePaths();
}

void FileManagerAPI::initializePaths() {
	mProtocols.clear();
	/*
	 * Defines paths
	 */
	
	// Internal
#if _WIN32
	boost::filesystem::path internalPath(FB::utf8_to_wstring(FB::BrowserPlugin::getFSPath()));
#else
	boost::filesystem::path internalPath(FB::BrowserPlugin::getFSPath());
#endif
	internalPath = internalPath.parent_path();
	internalPath /= std::string(PLUGIN_SHAREDIR);
	
	// Temp
#if _WIN32
	boost::filesystem::path tempPath(FB::utf8_to_wstring(FB::System::getTempPath()));
#else
	boost::filesystem::path tempPath(FB::System::getTempPath());
#endif
	tempPath /= FBSTRING_ProductDomain;
	tempPath /= boost::filesystem::unique_path();
	
	// Local
	boost::filesystem::path localPath = tempPath; // fallback
	FB::PluginCorePtr plugin = getFactory()->getPlugin();
	FB::URI loc = FB::URI::fromString(plugin->getHost()->getDOMWindow()->getLocation());
	if(!loc.domain.empty()) {
#if _WIN32
		std::wstring localPathString = FB::utf8_to_wstring(FB::System::getLocalAppDataPath(FBSTRING_ProductDomain));
#else
		std::string localPathString = FB::System::getLocalAppDataPath(FBSTRING_ProductDomain);
#endif
		if(!localPathString.empty()) {
			localPath = boost::filesystem::path(localPathString);
			localPath /= std::string(loc.domain);
		} else {
			FBLOG_WARN("FileManagerAPI::setFactory", "Can't find local path: using temp path");
		}
	} else {
		FBLOG_WARN("FileManagerAPI::setFactory", "Can't find browser url: using temp path");
	}
	
	// Create directories
	// boost::filesystem::create_directories(internalPath);
	boost::filesystem::create_directories(tempPath);
	boost::filesystem::create_directories(localPath);

	// Normalize
	internalPath = boost::filesystem::canonical(internalPath);
	tempPath = boost::filesystem::canonical(tempPath);
	localPath = boost::filesystem::canonical(localPath);

	// Make generics
	internalPath = boost::filesystem::path(internalPath.generic_string());
	tempPath = boost::filesystem::path(tempPath.generic_string());
	localPath = boost::filesystem::path(localPath.generic_string());

	
	mProtocols.push_back(Protocol(Protocol::internal, internalPath));
	mProtocols.push_back(Protocol(Protocol::temp, tempPath));
	mProtocols.push_back(Protocol(Protocol::local, localPath));
	
	
	// Order bigger path first
	mProtocols.sort(boost::bind(std::greater<std::size_t>(),
								boost::bind(&std::string::length, boost::bind(&boost::filesystem::path::string<std::string>, boost::bind(&Protocol::getPath, _1))),
								boost::bind(&std::string::length, boost::bind(&boost::filesystem::path::string<std::string>, boost::bind(&Protocol::getPath, _2)))
								)
								
	);

	
	for(std::list<Protocol>::iterator it = mProtocols.begin(); it != mProtocols.end(); ++it) {
		FBLOG_DEBUG("FileManagerAPI::setFactory", it->getProtocol() << "=" << it->getPath());
	}
}

bool FileManagerAPI::isSameHost(FB::URI const &uri) {
	FB::PluginCorePtr plugin = getFactory()->getPlugin();
	FB::URI hostUri = plugin->getHost()->getDOMWindow()->getLocation();
	bool ret = boost::iequals(hostUri.protocol, uri.protocol) &&
		boost::iequals(hostUri.domain, uri.domain);
	if(ret) {
		int port = uri.port;
		int hostPort = hostUri.port;
		if(port == 0) {
			if(boost::iequals(hostUri.protocol, Protocol::https)) {
				port = 443;
			} else if(boost::iequals(uri.protocol, Protocol::http)) {
				port = 80;
			}
		}
		if(hostPort == 0) {
			if(boost::iequals(hostUri.protocol, Protocol::https)) {
				hostPort = 443;
			} else if(boost::iequals(hostUri.protocol, Protocol::http)) {
				hostPort = 80;
			}
		}
		ret = (port == hostPort);
	}
	return ret;
}

bool FileManagerAPI::isHttp(FB::URI const &uri) {
	return boost::iequals(uri.protocol, Protocol::http) ||
	boost::iequals(uri.protocol, Protocol::https);
}

bool FileManagerAPI::isInternal(FB::URI const &uri) {
	return boost::iequals(uri.protocol, Protocol::internal);
}

bool FileManagerAPI::isFile(FB::URI const &uri) {
	return boost::iequals(uri.protocol, Protocol::internal) ||
		   boost::iequals(uri.protocol, Protocol::temp) ||
		   boost::iequals(uri.protocol, Protocol::local);
}

std::string FileManagerAPI::uriToFile(FB::URI const &uri) {
	try {
		// A domain it's not valid
		if(!uri.domain.empty()) {
			return std::string();
		}
		
		// Get the Protocol from protocol name
		std::list<Protocol>::iterator it = std::find_if(mProtocols.begin(),
								                        mProtocols.end(),
								                        boost::bind(std::equal_to<std::string>(), boost::ref(uri.protocol), boost::bind(&Protocol::getProtocol, _1)));
		if(it == mProtocols.end()) {
			return std::string();
		}
		std::string path = it->getPath().string();
		
		boost::filesystem::path absFile = boost::filesystem::normalize(boost::filesystem::path(path + uri.path));
		if(!boost::starts_with(absFile.string(), path)) {
			return std::string();
		}
		return absFile.make_preferred().string();
	} catch(boost::filesystem::filesystem_error &) {
		return std::string();
	}
}

FB::URI FileManagerAPI::fileToUri(std::string const &file) {
	try {
		FB::URI uri;
		std::string absFile = boost::filesystem::normalize(boost::filesystem::path(file)).generic_string();
		std::string path;
		
		// Get the Protocol with a path starting by the "absFile" file path
		std::list<Protocol>::iterator it = std::find_if(mProtocols.begin(),
								                        mProtocols.end(),
								                        boost::bind(boost::starts_with<std::string, std::string>, boost::ref(absFile), boost::bind(&boost::filesystem::path::string<std::string>, boost::bind(&Protocol::getPath, _1))));
		if(it == mProtocols.end()) {
			return FB::URI();
		}
		uri.protocol = it->getProtocol();

		// Troncate and clear up
		uri.path = absFile.substr(path.length());
		return uri;
	} catch(boost::filesystem::filesystem_error &) {
		return FB::URI();
	}
}

FileTransferAPIPtr FileManagerAPI::copy(std::string const &sourceUrl, std::string const &targetUrl, FB::JSObjectPtr const &callback) {
	FBLOG_DEBUG("FileManagerAPI::copy", "this=" << this << "\t" << "sourceUrl=" << sourceUrl << "\t" << "targetUrl=" << targetUrl << "\t" << "callback=" << callback);
	FB::URI sourceUri(sourceUrl);
	FB::URI targetUri(targetUrl);
	if(FileManagerAPI::isHttp(sourceUri) && FileManagerAPI::isHttp(targetUri)) {
		FBLOG_DEBUG("FileManagerAPI::copy", "Can't copy two Remote resources");
		callback->InvokeAsync("", FB::variant_list_of(false)("Can't copy two Remote resources"));
		return FileTransferAPIPtr();
	}
	if(FileManagerAPI::isInternal(targetUri)) {
		FBLOG_DEBUG("FileManagerAPI::copy", "Can't copy file into Internal resources");
		callback->InvokeAsync("", FB::variant_list_of(false)("Can't copy file into Internal resources"));
		return FileTransferAPIPtr();
	}
	if(FileManagerAPI::isHttp(sourceUri) && !isSameHost(sourceUri)) {
		FBLOG_DEBUG("FileManagerAPI::copy", "Can't use different host with remote resource (XSS)");
		callback->InvokeAsync("", FB::variant_list_of(false)("Can't use different host with remote resource (XSS)"));
		return FileTransferAPIPtr();
	}
	if(FileManagerAPI::isHttp(targetUri) && !isSameHost(targetUri)) {
		FBLOG_DEBUG("FileManagerAPI::copy", "Can't use different host with remote resource (XSS)");
		callback->InvokeAsync("", FB::variant_list_of(false)("Can't use different host with remote resource (XSS)"));
		return FileTransferAPIPtr();
	}
	FileTransferAPIPtr fileTransfer = getFactory()->getFileTransfer(sourceUri, targetUri, callback);
	return fileTransfer;
}


void FileManagerAPI::exists(std::string const &url, FB::JSObjectPtr const &callback) {
	FBLOG_DEBUG("FileManagerAPI::exists", "this=" << this << "\t" << "url=" << url << "\t" << "callback=" << callback);
	FB::URI uri(url);
	if(!FileManagerAPI::isFile(uri)) {
		FBLOG_DEBUG("FileManagerAPI::exists", "Can't check Remote resource");
		callback->InvokeAsync("", FB::variant_list_of(false)("Can't check Remote resource"));
		return;
	}
	try {
		std::string pathStr = uriToFile(uri);
		if(pathStr.empty()) {
			FBLOG_DEBUG("FileManagerAPI::exists", "Invalid path");
			callback->InvokeAsync("", FB::variant_list_of(false)("Invalid path"));
			return;
		}
		boost::filesystem::path path(pathStr);
		
		FBLOG_DEBUG("FileManagerAPI::exists", "Test \"" << pathStr << "\"");
		bool b = boost::filesystem::exists(path);
		callback->InvokeAsync("", FB::variant_list_of(b)(""));
	} catch(boost::filesystem::filesystem_error &) {
		FBLOG_DEBUG("FileManagerAPI::exists", "Internal error");
		callback->InvokeAsync("", FB::variant_list_of(false)("Internal error"));
	}
}

void FileManagerAPI::mkdir(std::string const &url, FB::JSObjectPtr const &callback) {
	FBLOG_DEBUG("FileManagerAPI::mkdir", "this=" << this << "\t" << "url=" << url << "\t" << "callback=" << callback);
	FB::URI uri(url);
	try {
		if(!FileManagerAPI::isFile(uri)) {
			FBLOG_DEBUG("FileManagerAPI::mkdir", "Can't create Remote directories");
			callback->InvokeAsync("", FB::variant_list_of(false)("Can't create Remote directories"));
			return;
		}
		if(FileManagerAPI::isInternal(uri)) {
			FBLOG_DEBUG("FileManagerAPI::mkdir", "Can't create Internal directories");
			callback->InvokeAsync("", FB::variant_list_of(false)("Can't create Internal directories"));
			return;
		}
		std::string pathStr = uriToFile(uri);
		if(pathStr.empty()) {
			FBLOG_DEBUG("FileManagerAPI::mkdir", "Invalid path");
			callback->InvokeAsync("", FB::variant_list_of(false)("Invalid path"));
			return;
		}
		boost::filesystem::path path(pathStr);
		if(boost::filesystem::exists(path)) {
			FBLOG_DEBUG("FileManagerAPI::mkdir", "The path \"" << pathStr << "\" already exists");
			callback->InvokeAsync("", FB::variant_list_of(false)("Invalid file"));
			return;
		}
		
		FBLOG_DEBUG("FileManagerAPI::mkdir", "Make directories \"" << pathStr << "\"");
		boost::filesystem::create_directories(path);
		callback->InvokeAsync("", FB::variant_list_of(true)(""));
	} catch(boost::filesystem::filesystem_error &) {
		FBLOG_DEBUG("FileManagerAPI::mkdir", "Internal error");
		callback->InvokeAsync("", FB::variant_list_of(false)("Internal error"));
	}
}

void FileManagerAPI::remove(std::string const &url, FB::JSObjectPtr const &callback) {
	FBLOG_DEBUG("FileManagerAPI::remove", "this=" << this << "\t" << "url=" << url << "\t" << "callback=" << callback);
	FB::URI uri(url);
	try {
		if(!FileManagerAPI::isFile(uri)) {
			FBLOG_DEBUG("FileManagerAPI::remove", "Can't remove Remote resource");
			callback->InvokeAsync("", FB::variant_list_of(false)("Can't remove Remote resource"));
			return;
		}
		if(FileManagerAPI::isInternal(uri)) {
			FBLOG_DEBUG("FileManagerAPI::remove", "Can't remove Internal resource");
			callback->InvokeAsync("", FB::variant_list_of(false)("Can't remove Internal resource"));
			return;
		}
		std::string pathStr = uriToFile(uri);
		if(pathStr.empty()) {
			FBLOG_DEBUG("FileManagerAPI::remove", "Invalid path");
			callback->InvokeAsync("", FB::variant_list_of(false)("Invalid path"));
			return;
		}
		boost::filesystem::path path(pathStr);
		if(!boost::filesystem::exists(path)) {
			FBLOG_DEBUG("FileManagerAPI::remove", "The path \"" << pathStr << "\" doesn't exist");
			callback->InvokeAsync("", FB::variant_list_of(false)("Invalid file"));
			return;
		}
		
		FBLOG_DEBUG("FileManagerAPI::remove", "Remove \"" << pathStr << "\"");
		boost::filesystem::remove_all(path);
		callback->InvokeAsync("", FB::variant_list_of(true)(""));
	} catch(boost::filesystem::filesystem_error &) {
		FBLOG_DEBUG("FileManagerAPI::remove", "Internal error");
		callback->InvokeAsync("", FB::variant_list_of(false)("Internal error"));
	}
}
	
} // LinphoneWeb

