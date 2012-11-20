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

// MISSING BOOST FILESYSTEM OPERATION
//  operations.cpp  --------------------------------------------------------------------//

//  Copyright 2002-2009 Beman Dawes
//  Copyright 2001 Dietmar Kuehl

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  See library home page at http://www.boost.org/libs/filesystem

//--------------------------------------------------------------------------------------// 


namespace boost {
namespace filesystem {
# ifdef BOOST_WINDOWS_API
  const boost::filesystem::path dot_path(L".");
  const boost::filesystem::path dot_dot_path(L"..");
# else
  const boost::filesystem::path dot_path(".");
  const boost::filesystem::path dot_dot_path("..");
# endif
  BOOST_FILESYSTEM_DECL
  path canonical(const path& p, const path& base = boost::filesystem::current_path(), system::error_code* ec=0)
  {
    path source (p.is_absolute() ? p : absolute(p, base));
    path result;

    boost::system::error_code local_ec;
    file_status stat (status(source, local_ec));

    if (stat.type() == boost::filesystem::file_not_found)
    {
      if (ec == 0)
        BOOST_FILESYSTEM_THROW(filesystem_error(
          "boost::filesystem::canonical", source,
          boost::system::error_code(boost::system::errc::no_such_file_or_directory, boost::system::generic_category())));
      ec->assign(boost::system::errc::no_such_file_or_directory, boost::system::generic_category());
      return result;
    }
    else if (local_ec)
    {
      if (ec == 0)
        BOOST_FILESYSTEM_THROW(filesystem_error(
          "boost::filesystem::canonical", source, local_ec));
      *ec = local_ec;
      return result;
    }

    bool scan (true);
    while (scan)
    {
      scan = false;
      result.clear();
      for (path::iterator itr = source.begin(); itr != source.end(); ++itr)
      {
        if (*itr == dot_path)
          continue;
        if (*itr == dot_dot_path)
        {
          result.remove_filename();
          continue;
        }

        result /= *itr;

        bool is_sym (boost::filesystem::is_symlink(boost::filesystem::symlink_status(result, *ec)));
        if (ec && *ec)
          return path();

        if (is_sym)
        {
          path link(boost::filesystem::read_symlink(result, *ec));
          if (ec && *ec)
            return path();
          result.remove_filename();

          if (link.is_absolute())
          {
            for (++itr; itr != source.end(); ++itr)
              link /= *itr;
            source = link;
          }
          else // link is relative
          {
            path new_source(result);
            new_source /= link;
            for (++itr; itr != source.end(); ++itr)
              new_source /= *itr;
            source = new_source;
          }
          scan = true;  // symlink causes scan to be restarted
          break;
        }
      }
    }
    if (ec != 0)
      ec->clear();
    //BOOST_ASSERT_MSG(result.is_absolute(), "canonical() implementation error; please report");
    return result;
  }
}
}
//--------------------------------------------------------------------------------------// 
//--------------------------------------------------------------------------------------// 


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
		return absFile.make_preferred().native();
	} catch(boost::filesystem::filesystem_error &err) {
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
		}
		return uri;
	} catch(boost::filesystem::filesystem_error &err) {
		return FB::URI();
	}
}
