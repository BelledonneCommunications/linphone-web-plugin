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

#include <JSAPIAuto.h>
#include <linphonecore.h>


FB_FORWARD_PTR(AddressAPI)
class AddressAPI: public FB::JSAPIAuto {
private:
	LinphoneAddress *mAddress;
	bool mUsed;

	AddressAPI(LinphoneAddress *address);
	AddressAPI(const std::string &uri);

	void initProxy();
public:
	~AddressAPI();

	std::string asString();
	std::string asStringUriOnly();
	void clean();
	AddressAPIPtr clone();
	std::string getDisplayName();
	std::string getDomain();
	std::string getPort();
	int getPortInt();
	std::string getScheme();
	std::string getUsername();
	void setDisplayName(const std::string &displayname);
	void setDomain(const std::string &domain);
	void setPort(const std::string &port);
	void setPortInt(int port);
	void setUsername(const std::string &username);

	inline LinphoneAddress *getRef() {
		return mAddress;
	}
	static AddressAPIPtr get(LinphoneAddress *address);
};

#endif //H_ADDRESSAPI
