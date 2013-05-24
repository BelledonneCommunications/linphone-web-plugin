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

#ifndef H_ADDRESSAPI
#define H_ADDRESSAPI

#include <linphonecore.h>
#include "wrapperapi.h"

namespace LinphoneWeb {

FB_FORWARD_PTR(AddressAPI)
class AddressAPI: public WrapperAPI {
	friend class FactoryAPI;
private:
	LinphoneAddress *mAddress;

	AddressAPI(LinphoneAddress *address);
	AddressAPI(const LinphoneAddress *address);
	AddressAPI(const StringPtr &uri);

protected:
	void initProxy();
	
public:
	~AddressAPI();

	StringPtr asString() const;
	StringPtr asStringUriOnly() const;
	void clean();
	AddressAPIPtr clone() const;
	StringPtr getDisplayName() const;
	StringPtr getDomain() const;
	StringPtr getPort() const;
	int getPortInt() const;
	StringPtr getScheme() const;
	StringPtr getUsername() const;
	void setDisplayName(const StringPtr &displayname);
	void setDomain(const StringPtr &domain);
	void setPort(const StringPtr &port);
	void setPortInt(int port);
	void setUsername(const StringPtr &username);

	inline LinphoneAddress *getRef() {
		mUsed = true;
		return mAddress;
	}
};
	
} // LinphoneWeb

#endif //H_ADDRESSAPI
