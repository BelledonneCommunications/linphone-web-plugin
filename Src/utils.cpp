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

#include "utils.h"
#include <BrowserPlugin.h>
#include <SystemHelpers.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <iterator>
#include <utf8.h>

namespace LinphoneWeb {
	
std::ostream& operator<<(std::ostream &out, StringPtr const &string) {
	if(string) {
		out << "\"" << string->c_str() << "\"";
	} else {
		out << "(NULL)";
	}
	return out;
}

StringPtr CHARPTR_TO_STRING(const char *cstr) {
	if(cstr != NULL) {
		std::string str(cstr);
		std::string filteredStr;
		
		// Replace invalid utf8 chars
		utf8::replace_invalid(str.begin(), str.end(), std::back_inserter(filteredStr));
		return StringPtr(filteredStr);
	}
	return StringPtr();
}

const char *STRING_TO_CHARPTR(StringPtr const &str) {
	return (str)? str->c_str() : NULL;
}
	
std::string PRINT_STRING(StringPtr const &str) {
	return (str)? str->c_str() : "(NULL)";
}
	
} // LinphoneWeb
