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

#include "authinfoapi.h"

#include "utils.h"
#include "factoryapi.h"

namespace LinphoneWeb {

AuthInfoAPI::AuthInfoAPI(LinphoneAuthInfo *authInfo) :
		WrapperAPI(APIDescription(this)), mAuthInfo(authInfo) {
	FBLOG_DEBUG("AuthInfoAPI::AuthInfoAPI", "this=" << this << "\t" << "authInfo=" << authInfo);
}

AuthInfoAPI::AuthInfoAPI(const LinphoneAuthInfo *authInfo) :
		WrapperAPI(APIDescription(this)), mAuthInfo(const_cast<LinphoneAuthInfo *>(authInfo)) {
	FBLOG_DEBUG("AuthInfoAPI::AuthInfoAPI", "this=" << this << "\t" << "authInfo=" << authInfo);
}


AuthInfoAPI::AuthInfoAPI(const StringPtr &username, const StringPtr &userid, const StringPtr &passwd, const StringPtr &ha1, const StringPtr &realm) :
		WrapperAPI(APIDescription(this)) {
	FBLOG_DEBUG("AuthInfoAPI::AuthInfoAPI",
			"this=" << this << "username=" << username << ", userid=" << userid << ", passwd=" << passwd << ", ha1" << ha1 << ", realm=" << realm);
	mAuthInfo = linphone_auth_info_new(STRING_TO_CHARPTR(username), STRING_TO_CHARPTR(userid), STRING_TO_CHARPTR(passwd), STRING_TO_CHARPTR(ha1), STRING_TO_CHARPTR(realm));
}

void AuthInfoAPI::initProxy() {
	registerProperty("ha1", make_property(this, &AuthInfoAPI::getHa1, &AuthInfoAPI::setHa1));
	registerProperty("realm", make_property(this, &AuthInfoAPI::getRealm, &AuthInfoAPI::setRealm));
	registerProperty("userid", make_property(this, &AuthInfoAPI::getUserid, &AuthInfoAPI::setUserid));
	registerProperty("username", make_property(this, &AuthInfoAPI::getUsername, &AuthInfoAPI::setUsername));
	registerProperty("passwd", make_property(this, &AuthInfoAPI::getPasswd, &AuthInfoAPI::setPasswd));
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

void AuthInfoAPI::setHa1(const StringPtr &ha1) {
	CORE_MUTEX
	
	FBLOG_DEBUG("AuthInfoAPI::setHa1", "this=" << this << "\t" << "ha1=" << ha1);
	linphone_auth_info_set_ha1(mAuthInfo, STRING_TO_CHARPTR(ha1));
}

StringPtr AuthInfoAPI::getRealm() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("AuthInfoAPI::getRealm", "this=" << this);
	return CHARPTR_TO_STRING(linphone_auth_info_get_realm(mAuthInfo));
}

void AuthInfoAPI::setRealm(const StringPtr &realm) {
	CORE_MUTEX
	
	FBLOG_DEBUG("AuthInfoAPI::setRealm", "this=" << this << "\t" << "realm=" << realm);
	linphone_auth_info_set_realm(mAuthInfo, STRING_TO_CHARPTR(realm));
}

StringPtr AuthInfoAPI::getUserid() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("AuthInfoAPI::getUserid", "this=" << this);
	return CHARPTR_TO_STRING(linphone_auth_info_get_userid(mAuthInfo));
}

void AuthInfoAPI::setUserid(const StringPtr &userid) {
	CORE_MUTEX
	
	FBLOG_DEBUG("AuthInfoAPI::setUserid", "this=" << this << "\t" << "userid=" << userid);
	linphone_auth_info_set_userid(mAuthInfo, STRING_TO_CHARPTR(userid));
}

StringPtr AuthInfoAPI::getUsername() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("AuthInfoAPI::getUsername", "this=" << this);
	return CHARPTR_TO_STRING(linphone_auth_info_get_username(mAuthInfo));
}

void AuthInfoAPI::setUsername(const StringPtr &username) {
	CORE_MUTEX
	
	FBLOG_DEBUG("AuthInfoAPI::setUsername", "this=" << this << "\t" << "username=" << username);
	linphone_auth_info_set_username(mAuthInfo, STRING_TO_CHARPTR(username));
}

StringPtr AuthInfoAPI::getPasswd() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("AuthInfoAPI::getPasswd", "this=" << this);
	return CHARPTR_TO_STRING(linphone_auth_info_get_passwd(mAuthInfo));
}

void AuthInfoAPI::setPasswd(const StringPtr &passwd) {
	CORE_MUTEX
	
	FBLOG_DEBUG("AuthInfoAPI::setPasswd", "this=" << this << "\t" << "passwd=" << passwd);
	linphone_auth_info_set_passwd(mAuthInfo, STRING_TO_CHARPTR(passwd));
}
	
} // LinphoneWeb
