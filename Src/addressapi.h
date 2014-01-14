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

#ifndef H_ADDRESSAPI
#define H_ADDRESSAPI

#include <linphone/linphonecore.h>
#include "wrapperapi.h"

namespace LinphoneWeb {

FB_FORWARD_PTR(AddressAPI)
class AddressAPI: public WrapperAPI {
	friend class FactoryAPI;
private:
	LinphoneAddress *mAddress;

	AddressAPI(LinphoneAddress *address);
	AddressAPI(const LinphoneAddress *address);
	AddressAPI(StringPtr const &uri);

protected:
	virtual void initProxy();
	
public:
	virtual ~AddressAPI();

	StringPtr asString() const;
	StringPtr asStringUriOnly() const;
	void clean();
	AddressAPIPtr clone() const;
	StringPtr getDisplayName() const;
	StringPtr getDomain() const;
	int getPort() const;
	StringPtr getScheme() const;
	int getTransport() const;
	StringPtr getUsername() const;
	void setDisplayName(StringPtr const &displayname);
	void setDomain(StringPtr const &domain);
	void setPort(int port);
	void setTransport(int transport);
	void setUsername(StringPtr const &username);

	inline LinphoneAddress *getRef() {
		return mAddress;
	}
};
	
} // LinphoneWeb

#endif //H_ADDRESSAPI
