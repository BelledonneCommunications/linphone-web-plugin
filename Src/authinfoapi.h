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

#ifndef H_AUTHINFOAPI
#define H_AUTHINFOAPI

#include <linphone/linphonecore.h>
#include "wrapperapi.h"

namespace LinphoneWeb {

FB_FORWARD_PTR(AuthInfoAPI)
class AuthInfoAPI: public WrapperAPI {
	friend class FactoryAPI;
private:
	LinphoneAuthInfo *mAuthInfo;

	AuthInfoAPI(LinphoneAuthInfo *authInfo);
	AuthInfoAPI(const LinphoneAuthInfo *authInfo);
	AuthInfoAPI(StringPtr const &username, StringPtr const &userid,
				StringPtr const &passwd, StringPtr const &ha1, StringPtr const &realm, StringPtr const &domain);
	
protected:
	virtual void initProxy();
	
public:
	virtual ~AuthInfoAPI();

	StringPtr getHa1() const;
	void setHa1(StringPtr const &ha1);
	
	StringPtr getRealm() const;
	void setRealm(StringPtr const &realm);
	
	StringPtr getUserid() const;
	void setUserid(StringPtr const &userid);

	StringPtr getUsername() const;
	void setUsername(StringPtr const &username);

	StringPtr getPasswd() const;
	void setPasswd(StringPtr const &passwd);
	
	StringPtr getDomain() const;
	void setDomain(StringPtr const &domain);

	inline LinphoneAuthInfo *getRef() {
		return mAuthInfo;
	}

};
	
} // LinphoneWeb

#endif //H_AUTHINFOAPI
