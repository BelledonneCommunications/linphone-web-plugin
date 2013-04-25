/*!
 Linphone Web - Web plugin of Linphone an audio/video SIP phone
 Copyright (C) 2012  Yann Diorcet <yann.diorcet@linphone.org>
 
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
	    boost::filesystem::path normalize(const boost::filesystem::path &path) {
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


/*
 * Protocol
 */

const std::string FileManagerAPI::Protocol::http("http");
const std::string FileManagerAPI::Protocol::https("https");
const std::string FileManagerAPI::Protocol::internal("internal");
const std::string FileManagerAPI::Protocol::temp("tmp");
const std::string FileManagerAPI::Protocol::local("local");

FileManagerAPI::Protocol::Protocol(const std::string &protocol, const boost::filesystem::path& path): mProtocol(protocol), mPath(path) {
}

void FileManagerAPI::initProxy() {
	registerMethod("copy", make_method(this, &FileManagerAPI::copy));
	registerMethod("exists", make_method(this, &FileManagerAPI::exists));
	registerMethod("mkdir", make_method(this, &FileManagerAPI::mkdir));
	registerMethod("remove", make_method(this, &FileManagerAPI::remove));
}

const std::string &FileManagerAPI::Protocol::getProtocol() {
	return mProtocol;
}

const boost::filesystem::path &FileManagerAPI::Protocol::getPath() {
	return mPath;
}


/*
 * FileManagerAPI
 */

FileManagerAPI::FileManagerAPI():
	WrapperAPI(APIDescription(this)) {
	initProxy();
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
	boost::filesystem::path internalPath = FB::BrowserPlugin::getFSPath();
	internalPath = internalPath.parent_path();
	internalPath /= std::string("linphoneweb");
	
	// Temp
	boost::filesystem::path tempPath = boost::filesystem::path(FB::System::getTempPath());
	tempPath /= FBSTRING_ProductName;
	tempPath /= boost::filesystem::unique_path();
	
	// Local
	boost::filesystem::path localPath = tempPath; // fallback
	CorePluginPtr plugin = mFactory->getPlugin();
	FB::URI loc = FB::URI::fromString(plugin->getHost()->getDOMWindow()->getLocation());
	if(!loc.domain.empty()) {
	    std::string localPathString = FB::System::getAppDataPath(FBSTRING_ProductName);
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

bool FileManagerAPI::isHttp(const FB::URI &uri) {
	return boost::iequals(uri.protocol, Protocol::http) ||
	boost::iequals(uri.protocol, Protocol::https);
}

bool FileManagerAPI::isInternal(const FB::URI &uri) {
	return boost::iequals(uri.protocol, Protocol::internal);
}

bool FileManagerAPI::isFile(const FB::URI &uri) {
	return boost::iequals(uri.protocol, Protocol::internal) ||
	       boost::iequals(uri.protocol, Protocol::temp) ||
	       boost::iequals(uri.protocol, Protocol::local);
}

std::string FileManagerAPI::uriToFile(const FB::URI &uri) {
	try {
	    // A domain it's not valid
	    if(!uri.domain.empty()) {
	        return std::string();
	    }
	    
	    // boost::iequals(uri.protocol, internalProtocol)
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

FB::URI FileManagerAPI::fileToUri(const std::string &file) {
	try {
		FB::URI uri;
		std::string absFile = boost::filesystem::normalize(boost::filesystem::path(file)).generic_string();
		std::string path;
	    
	    // boost::starts_with(absFile, path)
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

FileTransferAPIPtr FileManagerAPI::copy(const std::string &sourceUrl, const std::string &targetUrl, const FB::JSObjectPtr& callback) {
	FBLOG_DEBUG("FileManagerAPI::copy()", "this=" << this << "\t" << "sourceUrl=" << sourceUrl << "\t" << "targetUrl=" << targetUrl << "\t" << "callback=" << callback);
	FB::URI sourceUri(sourceUrl);
	FB::URI targetUri(targetUrl);
	if(FileManagerAPI::isHttp(sourceUri) && FileManagerAPI::isHttp(targetUri)) {
	    FBLOG_DEBUG("FileManagerAPI::copy()", "Can't copy two Remote resources");
	    callback->InvokeAsync("", FB::variant_list_of(false)("Can't copy two Remote resources"));
	    return FileTransferAPIPtr();
	}
	if(FileManagerAPI::isInternal(targetUri)) {
	    FBLOG_DEBUG("FileManagerAPI::mkdir()", "Can't copy file into Internal resources");
	    callback->InvokeAsync("", FB::variant_list_of(false)("Can't copy file into Internal resources"));
	    return FileTransferAPIPtr();
	}
	FileTransferAPIPtr fileTransfer = mFactory->getFileTransfer(sourceUri, targetUri, callback);
	return fileTransfer;
}


void FileManagerAPI::exists(const std::string &url, const FB::JSObjectPtr& callback) {
	FBLOG_DEBUG("FileManagerAPI::exists()", "this=" << this << "\t" << "url=" << url << "\t" << "callback=" << callback);
	FB::URI uri(url);
	if(!FileManagerAPI::isFile(uri)) {
	    FBLOG_DEBUG("FileManagerAPI::exists()", "Can't check Remote resource");
	    callback->InvokeAsync("", FB::variant_list_of(false)("Can't check Remote resource"));
	    return;
	}
	try {
	    FB::URI uri(url);
	    std::string pathStr = uriToFile(uri);
	    if(pathStr.empty()) {
	        FBLOG_DEBUG("FileManagerAPI::exists()", "Invalid path");
	        callback->InvokeAsync("", FB::variant_list_of(false)("Invalid path"));
	        return;
	    }
	    boost::filesystem::path path(pathStr);
	    
	    FBLOG_DEBUG("FileManagerAPI::exists()", "Test \"" << pathStr << "\"");
	    callback->InvokeAsync("", FB::variant_list_of(boost::filesystem::exists(path))(NULL));
	} catch(boost::filesystem::filesystem_error &) {
	    FBLOG_DEBUG("FileManagerAPI::exists()", "Internal error");
	    callback->InvokeAsync("", FB::variant_list_of(false)("Internal error"));
	}
}

void FileManagerAPI::mkdir(const std::string &url, const FB::JSObjectPtr& callback) {
	FBLOG_DEBUG("FileManagerAPI::mkdir()", "this=" << this << "\t" << "url=" << url << "\t" << "callback=" << callback);
	FB::URI uri(url);
	try {
	    if(!FileManagerAPI::isFile(uri)) {
	        FBLOG_DEBUG("FileManagerAPI::mkdir()", "Can't create Remote directories");
	        callback->InvokeAsync("", FB::variant_list_of(false)("Can't create Remote directories"));
	        return;
	    }
	    if(FileManagerAPI::isInternal(uri)) {
	        FBLOG_DEBUG("FileManagerAPI::mkdir()", "Can't create Internal directories");
	        callback->InvokeAsync("", FB::variant_list_of(false)("Can't create Internal directories"));
	        return;
	    }
	    std::string pathStr = uriToFile(uri);
	    if(pathStr.empty()) {
	        FBLOG_DEBUG("FileManagerAPI::mkdir()", "Invalid path");
	        callback->InvokeAsync("", FB::variant_list_of(false)("Invalid path"));
	        return;
	    }
	    boost::filesystem::path path(pathStr);
	    if(boost::filesystem::exists(path)) {
	        FBLOG_DEBUG("FileManagerAPI::mkdir()", "The path \"" << pathStr << "\" already exists");
	        callback->InvokeAsync("", FB::variant_list_of(false)("Invalid file"));
	        return;
	    }
	    
	    FBLOG_DEBUG("FileManagerAPI::mkdir()", "Make directories \"" << pathStr << "\"");
	    boost::filesystem::create_directories(path);
	    callback->InvokeAsync("", FB::variant_list_of(true)(NULL));
	} catch(boost::filesystem::filesystem_error &) {
	    FBLOG_DEBUG("FileManagerAPI::mkdir()", "Internal error");
	    callback->InvokeAsync("", FB::variant_list_of(false)("Internal error"));
	}
}

void FileManagerAPI::remove(const std::string &url, const FB::JSObjectPtr& callback) {
	FBLOG_DEBUG("FileManagerAPI::remove()", "this=" << this << "\t" << "url=" << url << "\t" << "callback=" << callback);
	FB::URI uri(url);
	try {
	    if(!FileManagerAPI::isFile(uri)) {
	        FBLOG_DEBUG("FileManagerAPI::remove()", "Can't remove Remote resource");
	        callback->InvokeAsync("", FB::variant_list_of(false)("Can't remove Remote resource"));
	        return;
	    }
	    if(FileManagerAPI::isInternal(uri)) {
	        FBLOG_DEBUG("FileManagerAPI::remove()", "Can't remove Internal resource");
	        callback->InvokeAsync("", FB::variant_list_of(false)("Can't remove Internal resource"));
	        return;
	    }
	    std::string pathStr = uriToFile(uri);
	    if(pathStr.empty()) {
	        FBLOG_DEBUG("FileManagerAPI::remove()", "Invalid path");
	        callback->InvokeAsync("", FB::variant_list_of(false)("Invalid path"));
	        return;
	    }
	    boost::filesystem::path path(pathStr);
	    if(!boost::filesystem::exists(path)) {
	        FBLOG_DEBUG("FileManagerAPI::remove()", "The path \"" << pathStr << "\" doesn't exist");
	        callback->InvokeAsync("", FB::variant_list_of(false)("Invalid file"));
	        return;
	    }
	    
	    FBLOG_DEBUG("FileManagerAPI::remove()", "Remove \"" << pathStr << "\"");
	    boost::filesystem::remove_all(path);
	    callback->InvokeAsync("", FB::variant_list_of(true)(NULL));
	} catch(boost::filesystem::filesystem_error &) {
	    FBLOG_DEBUG("FileManagerAPI::remove()", "Internal error");
	    callback->InvokeAsync("", FB::variant_list_of(false)("Internal error"));
	}
}

