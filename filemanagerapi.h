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

#ifndef H_FILEMANAGERAPI
#define H_FILEMANAGERAPI

#include <JSAPIAuto.h>
#include <linphonecore.h>
#include "wrapperapi.h"

#include <URI.h>

#include <list>
#include <boost/filesystem.hpp>

FB_FORWARD_PTR(FileManagerAPI)
class FileManagerAPI: public FB::JSAPIAuto, public WrapperAPI {
    friend class FactoryAPI;
private:   
    FileManagerAPI();
    void initProxy();
    void initializePaths();
    
    class Protocol {
    private:
        std::string mProtocol;
        boost::filesystem::path mPath;
        
    public:
        Protocol(const std::string &protocol, const boost::filesystem::path& path);
        const std::string &getProtocol();
        const boost::filesystem::path &getPath();
        
    public:
        static const std::string http;
        static const std::string https;
        static const std::string internal;
        static const std::string temp;
        static const std::string local;
    };
    
    std::list<Protocol> mProtocols;
    
protected:
    virtual void setFactory(FactoryAPIPtr factory);
    
public:
    static bool isFile(const FB::URI &uri);
    static bool isHttp(const FB::URI &uri);
    std::string uriToFile(const FB::URI &uri);
    FB::URI fileToUri(const std::string &file);
};

#endif // H_FILEMANAGERAPI