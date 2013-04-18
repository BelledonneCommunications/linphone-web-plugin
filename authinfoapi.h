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

#ifndef H_AUTHINFOAPI
#define H_AUTHINFOAPI

#include <JSAPIAuto.h>
#include <linphonecore.h>
#include "wrapperapi.h"

FB_FORWARD_PTR(AuthInfoAPI)
class AuthInfoAPI: public FB::JSAPIAuto, public WrapperAPI {
    friend class FactoryAPI;
private:
	LinphoneAuthInfo *mAuthInfo;

	AuthInfoAPI(LinphoneAuthInfo *authInfo);
	AuthInfoAPI(const LinphoneAuthInfo *authInfo);
	void initProxy();
public:
	AuthInfoAPI(const std::string &username, const std::string &userid,
			const std::string &passwd, const std::string &ha1, const std::string &realm);
	~AuthInfoAPI();

	std::string getUserid() const;
	void setUserid(const std::string &userid);

	std::string getUsername() const;
	void setUsername(const std::string &username);

	std::string getPasswd() const;
	void setPasswd(const std::string &passwd);

	inline LinphoneAuthInfo *getRef() {
		mUsed = true;
		return mAuthInfo;
	}

};

#endif //H_AUTHINFOAPI
