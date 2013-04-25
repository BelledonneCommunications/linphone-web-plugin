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

FB_FORWARD_PTR(AddressAPI)
class AddressAPI: public WrapperAPI {
	friend class FactoryAPI;
private:
	LinphoneAddress *mAddress;

	AddressAPI(LinphoneAddress *address);
	AddressAPI(const LinphoneAddress *address);
	AddressAPI(const std::string &uri);

protected:
	void initProxy();
	
public:
	~AddressAPI();

	std::string asString() const;
	std::string asStringUriOnly() const;
	void clean();
	AddressAPIPtr clone() const;
	std::string getDisplayName() const;
	std::string getDomain() const;
	std::string getPort() const;
	int getPortInt() const;
	std::string getScheme() const;
	std::string getUsername() const;
	void setDisplayName(const std::string &displayname);
	void setDomain(const std::string &domain);
	void setPort(const std::string &port);
	void setPortInt(int port);
	void setUsername(const std::string &username);

	inline LinphoneAddress *getRef() {
		mUsed = true;
		return mAddress;
	}
};

#endif //H_ADDRESSAPI
