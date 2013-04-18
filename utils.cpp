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

#include "utils.h"
#include <BrowserPlugin.h>
#include <SystemHelpers.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

const std::string LINPHONE_WEB_INTERNAL_PROTOCOL("internal");
const std::string LINPHONE_WEB_TEMP_PROTOCOL("tmp");

std::string CHARPTR_TO_STRING(const char *str) {
	return (str != NULL) ? std::string(str) : std::string();
}

bool URI_IS_HTTP(const FB::URI &uri) {
	return boost::iequals(uri.protocol, "http") || 
		boost::iequals(uri.protocol, "https");
}

bool URI_IS_FILE(const FB::URI &uri) {
	return boost::iequals(uri.protocol, LINPHONE_WEB_INTERNAL_PROTOCOL) || 
		boost::iequals(uri.protocol, LINPHONE_WEB_TEMP_PROTOCOL);
}

std::string URI_TO_FILE(const FB::URI &uri) {
	try {
		std::string path;
		if(boost::iequals(uri.protocol, LINPHONE_WEB_INTERNAL_PROTOCOL)) {
			//path = boost::filesystem::canonical(boost::filesystem::path(FB::BrowserPlugin::getFSPath())).string();
			path = boost::filesystem::canonical(boost::filesystem::current_path()).string();	
		} else if(boost::iequals(uri.protocol, LINPHONE_WEB_TEMP_PROTOCOL)) {
			path = boost::filesystem::canonical(boost::filesystem::path(FB::System::getTempPath())).string();
		}
		if(path.empty()) {
			return std::string();
		}

		boost::filesystem::path absFile = boost::filesystem::canonical(boost::filesystem::path(path + uri.path)).string();
		if(!boost::starts_with(absFile.string(), path)) {
			return std::string();
		}
		return absFile.make_preferred().string();
	} catch(boost::filesystem::filesystem_error &) {
		return std::string();
	}
}

FB::URI FILE_TO_URI(const std::string &file) {
	try {
		FB::URI uri;
		std::string absFile = boost::filesystem::canonical(boost::filesystem::path(file)).string();
		std::string path;
		//path = boost::filesystem::canonical(boost::filesystem::path(FB::BrowserPlugin::getFSPath())).string();
		path = boost::filesystem::canonical(boost::filesystem::current_path()).string();
		if(boost::starts_with(absFile, path)) {
			uri.protocol = LINPHONE_WEB_INTERNAL_PROTOCOL;
		}
		if(uri.protocol.empty()) {
			path = boost::filesystem::canonical(boost::filesystem::path(FB::System::getTempPath())).string();
			if(boost::starts_with(absFile, path)) {
				uri.protocol = LINPHONE_WEB_TEMP_PROTOCOL;
			}
		}
		if(uri.protocol.empty()) {
			return FB::URI();
		} else {
			uri.path = absFile.substr(path.length());
			std::replace(uri.path.begin(), uri.path.end(), '\\', '/');
		}
		return uri;
	} catch(boost::filesystem::filesystem_error &) {
		return FB::URI();
	}
}
