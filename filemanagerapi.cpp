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
#include <SystemHelpers.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <boost/bind/protect.hpp>
#include <boost/function.hpp>

const std::string FileManagerAPI::Protocol::http("http");
const std::string FileManagerAPI::Protocol::https("https");
const std::string FileManagerAPI::Protocol::internal("internal");
const std::string FileManagerAPI::Protocol::temp("tmp");
const std::string FileManagerAPI::Protocol::local("local");

FileManagerAPI::Protocol::Protocol(const std::string &protocol, const boost::filesystem::path& path): mProtocol(protocol), mPath(path) {
    
}

const std::string &FileManagerAPI::Protocol::getProtocol() {
    return mProtocol;
}

const boost::filesystem::path &FileManagerAPI::Protocol::getPath() {
    return mPath;
}

FileManagerAPI::FileManagerAPI() {
    initProxy();
}

void FileManagerAPI::initProxy() {
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

bool FileManagerAPI::isFile(const FB::URI &uri) {
	return boost::iequals(uri.protocol, Protocol::internal) ||
           boost::iequals(uri.protocol, Protocol::temp) ||
           boost::iequals(uri.protocol, Protocol::local);
}

std::string FileManagerAPI::uriToFile(const FB::URI &uri) {
	try {
		std::string path;
        // boost::iequals(uri.protocol, internalProtocol)
        std::list<Protocol>::iterator it = std::find_if(mProtocols.begin(),
                                                        mProtocols.end(),
                                                        boost::bind(std::equal_to<std::string>(), boost::ref(uri.protocol), boost::bind(&Protocol::getProtocol, _1)));
        if(it == mProtocols.end()) {
            return std::string();
        }
        path = it->getPath().string();
        
		boost::filesystem::path absFile = boost::filesystem::canonical(boost::filesystem::path(path + uri.path));
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
		std::string absFile = boost::filesystem::canonical(boost::filesystem::path(file)).generic_string();
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

