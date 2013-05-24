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

#include <linphonecore.h>
#include "wrapperapi.h"

namespace LinphoneWeb {

FB_FORWARD_PTR(AuthInfoAPI)
class AuthInfoAPI: public WrapperAPI {
	friend class FactoryAPI;
private:
	LinphoneAuthInfo *mAuthInfo;

	AuthInfoAPI(LinphoneAuthInfo *authInfo);
	AuthInfoAPI(const LinphoneAuthInfo *authInfo);
	
protected:
	void initProxy();
	
public:
	AuthInfoAPI(const StringPtr &username, const StringPtr &userid,
			const StringPtr &passwd, const StringPtr &ha1, const StringPtr &realm);
	~AuthInfoAPI();

	StringPtr getHa1() const;
	void setHa1(const StringPtr &ha1);
	
	StringPtr getRealm() const;
	void setRealm(const StringPtr &realm);
	
	StringPtr getUserid() const;
	void setUserid(const StringPtr &userid);

	StringPtr getUsername() const;
	void setUsername(const StringPtr &username);

	StringPtr getPasswd() const;
	void setPasswd(const StringPtr &passwd);

	inline LinphoneAuthInfo *getRef() {
		mUsed = true;
		return mAuthInfo;
	}

};
	
} // LinphoneWeb

#endif //H_AUTHINFOAPI
