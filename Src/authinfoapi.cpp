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

#include "authinfoapi.h"

#include "utils.h"
#include "factoryapi.h"

#include <stdexcept>

namespace LinphoneWeb {

AuthInfoAPI::AuthInfoAPI(LinphoneAuthInfo *authInfo) :
		WrapperAPI(APIDescription(this)), mAuthInfo(authInfo) {
	FBLOG_DEBUG("AuthInfoAPI::AuthInfoAPI", "this=" << this << "\t" << "authInfo=" << authInfo);
}

AuthInfoAPI::AuthInfoAPI(const LinphoneAuthInfo *authInfo) :
		WrapperAPI(APIDescription(this)), mAuthInfo(const_cast<LinphoneAuthInfo *>(authInfo)) {
	FBLOG_DEBUG("AuthInfoAPI::AuthInfoAPI", "this=" << this << "\t" << "authInfo=" << authInfo);
}


AuthInfoAPI::AuthInfoAPI(StringPtr const &username, StringPtr const &userid, StringPtr const &passwd, StringPtr const &ha1, StringPtr const &realm, StringPtr const &domain) :
		WrapperAPI(APIDescription(this)) {
	FBLOG_DEBUG("AuthInfoAPI::AuthInfoAPI",
			"this=" << this << "\t" << "username=" << username << "\t" << "userid=" << userid << "\t" << "passwd=" << passwd << "\t" << "ha1" << ha1 << "\t" << "realm=" << realm << "\t" << "domain=" << domain);
	mAuthInfo = linphone_auth_info_new(STRING_TO_CHARPTR(username), STRING_TO_CHARPTR(userid), STRING_TO_CHARPTR(passwd), STRING_TO_CHARPTR(ha1), STRING_TO_CHARPTR(realm), STRING_TO_CHARPTR(domain));
	if(mAuthInfo == NULL) {
		throw std::invalid_argument("one/many parameters is/are invalid");
	}
}

void AuthInfoAPI::initProxy() {
	registerProperty("ha1", make_property(this, &AuthInfoAPI::getHa1, &AuthInfoAPI::setHa1));
	registerProperty("realm", make_property(this, &AuthInfoAPI::getRealm, &AuthInfoAPI::setRealm));
	registerProperty("userid", make_property(this, &AuthInfoAPI::getUserid, &AuthInfoAPI::setUserid));
	registerProperty("username", make_property(this, &AuthInfoAPI::getUsername, &AuthInfoAPI::setUsername));
	registerProperty("passwd", make_property(this, &AuthInfoAPI::getPasswd, &AuthInfoAPI::setPasswd));
	registerProperty("domain", make_property(this, &AuthInfoAPI::getDomain, &AuthInfoAPI::setDomain));
}

AuthInfoAPI::~AuthInfoAPI() {
	FBLOG_DEBUG("AuthInfoAPI::~AuthInfoAPI", "this=" << this);
	if(isOwned()) {
		if(mAuthInfo != NULL) {
			linphone_auth_info_destroy(mAuthInfo);
		}
	}
}

StringPtr AuthInfoAPI::getHa1() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("AuthInfoAPI::getHa1", "this=" << this);
	return CHARPTR_TO_STRING(linphone_auth_info_get_ha1(mAuthInfo));
}

void AuthInfoAPI::setHa1(StringPtr const &ha1) {
	CORE_MUTEX
	
	FBLOG_DEBUG("AuthInfoAPI::setHa1", "this=" << this << "\t" << "ha1=" << ha1);
	linphone_auth_info_set_ha1(mAuthInfo, STRING_TO_CHARPTR(ha1));
}

StringPtr AuthInfoAPI::getRealm() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("AuthInfoAPI::getRealm", "this=" << this);
	return CHARPTR_TO_STRING(linphone_auth_info_get_realm(mAuthInfo));
}

void AuthInfoAPI::setRealm(StringPtr const &realm) {
	CORE_MUTEX
	
	FBLOG_DEBUG("AuthInfoAPI::setRealm", "this=" << this << "\t" << "realm=" << realm);
	linphone_auth_info_set_realm(mAuthInfo, STRING_TO_CHARPTR(realm));
}

StringPtr AuthInfoAPI::getUserid() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("AuthInfoAPI::getUserid", "this=" << this);
	return CHARPTR_TO_STRING(linphone_auth_info_get_userid(mAuthInfo));
}

void AuthInfoAPI::setUserid(StringPtr const &userid) {
	CORE_MUTEX
	
	FBLOG_DEBUG("AuthInfoAPI::setUserid", "this=" << this << "\t" << "userid=" << userid);
	linphone_auth_info_set_userid(mAuthInfo, STRING_TO_CHARPTR(userid));
}

StringPtr AuthInfoAPI::getUsername() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("AuthInfoAPI::getUsername", "this=" << this);
	return CHARPTR_TO_STRING(linphone_auth_info_get_username(mAuthInfo));
}

void AuthInfoAPI::setUsername(StringPtr const &username) {
	CORE_MUTEX
	
	FBLOG_DEBUG("AuthInfoAPI::setUsername", "this=" << this << "\t" << "username=" << username);
	linphone_auth_info_set_username(mAuthInfo, STRING_TO_CHARPTR(username));
}

StringPtr AuthInfoAPI::getPasswd() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("AuthInfoAPI::getPasswd", "this=" << this);
	return CHARPTR_TO_STRING(linphone_auth_info_get_passwd(mAuthInfo));
}

void AuthInfoAPI::setPasswd(StringPtr const &passwd) {
	CORE_MUTEX
	
	FBLOG_DEBUG("AuthInfoAPI::setPasswd", "this=" << this << "\t" << "passwd=" << passwd);
	linphone_auth_info_set_passwd(mAuthInfo, STRING_TO_CHARPTR(passwd));
}

StringPtr AuthInfoAPI::getDomain() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("AuthInfoAPI::getDomain", "this=" << this);
	return CHARPTR_TO_STRING(linphone_auth_info_get_domain(mAuthInfo));
}

void AuthInfoAPI::setDomain(StringPtr const &domain) {
	CORE_MUTEX
	
	FBLOG_DEBUG("AuthInfoAPI::setDomain", "this=" << this << "\t" << "domain=" << domain);
	linphone_auth_info_set_domain(mAuthInfo, STRING_TO_CHARPTR(domain));
}
	
} // LinphoneWeb
