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

#ifndef H_FILEMANAGERAPI
#define H_FILEMANAGERAPI

#include "macro.h"
#include <linphone/linphonecore.h>
#include "wrapperapi.h"

#include <URI.h>

#include <list>
#include <boost/filesystem.hpp>

namespace LinphoneWeb {

FB_FORWARD_PTR(FileTransferAPI)

FB_FORWARD_PTR(FileManagerAPI)
class FileManagerAPI: public WrapperAPI {
	friend class FactoryAPI;
private:   
	FileManagerAPI();
	void initializePaths();
	
	/*
	 * < Protocol
	 */
	class Protocol {
	private:
		std::string mProtocol;
		boost::filesystem::path mPath;
		
	public:
		Protocol(std::string const &protocol, boost::filesystem::path const &path);
		std::string const &getProtocol();
		boost::filesystem::path const &getPath();
		
	public:
		static const std::string http;
		static const std::string https;
		static const std::string internal;
		static const std::string temp;
		static const std::string local;
	};
	/*
	 * Protocol >
	 */
	
	std::list<Protocol> mProtocols;
	
protected:
	virtual void setFactory(FactoryAPIPtr factory);
	
public:
	bool isSameHost(FB::URI const &uri);
	static bool isInternal(FB::URI const &uri);
	static bool isFile(FB::URI const &uri);
	static bool isHttp(FB::URI const &uri);
	std::string uriToFile(FB::URI const &uri);
	FB::URI fileToUri(std::string const &file);
	
protected:
	virtual void initProxy();
	
public:
	virtual ~FileManagerAPI();
	
	FileTransferAPIPtr copy(std::string const &sourceUrl, std::string const &targetUrl, FB::JSObjectPtr const &callback);
	void exists(std::string const &url, FB::JSObjectPtr const &callback);
	void remove(std::string const &url, FB::JSObjectPtr const &callback);
	void mkdir(std::string const &url, FB::JSObjectPtr const &callback);
};
	
} // LinphoneWeb

#endif // H_FILEMANAGERAPI
