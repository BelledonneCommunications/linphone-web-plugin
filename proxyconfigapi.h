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

#ifndef H_PROXYCONFIGAPI
#define H_PROXYCONFIGAPI

#include <linphonecore.h>
#include "wrapperapi.h"

namespace LinphoneWeb {

FB_FORWARD_PTR(CoreAPI)

FB_FORWARD_PTR(ProxyConfigAPI)
class ProxyConfigAPI: public WrapperAPI {
	friend class FactoryAPI;
private:
	LinphoneProxyConfig *mProxyConfig;

	ProxyConfigAPI(LinphoneProxyConfig *proxyConfig);
	
protected:
	void initProxy();
	
public:
	ProxyConfigAPI();
	~ProxyConfigAPI();

	CoreAPIPtr getCore() const;

	StringPtr getContactParameters() const;
	void setContactParameters(const StringPtr &parameter);

	bool getDialEscapePlus() const;
	void setDialEscapePlus(bool escape);

	StringPtr getDialPrefix() const;
	void setDialPrefix(const StringPtr &prefix);

	StringPtr getDomain() const;
	int getError() const;

	bool publishEnabled() const;
	void enablePublish(bool enable);

	StringPtr getSipSetup() const;
	void setSipSetup(const StringPtr &sip_setup);

	bool isRegistered() const;
	//getSipSetupContext

	void setServerAddr(const StringPtr &server_addr);
	StringPtr getServerAddr() const;

	void setIdentity(const StringPtr &identity);
	StringPtr getIdentity() const;

	void setRoute(const StringPtr &route);
	StringPtr getRoute() const;

	void setExpires(int expires);
	int getExpires() const;

	void enableRegister(bool val);
	bool registerEnabled() const;

	int getState() const;

	void edit();
	int done();

	StringPtr normalizeNumber(const StringPtr &username) const;
	void refreshRegister();

	inline LinphoneProxyConfig *getRef() {
		mUsed = true;
		return mProxyConfig;
	}
};
	
} // LinphoneWeb

#endif //H_PROXYCONFIGAPI
