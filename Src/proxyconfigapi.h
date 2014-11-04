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

#ifndef H_PROXYCONFIGAPI
#define H_PROXYCONFIGAPI

#include <linphone/linphonecore.h>
#include "wrapperapi.h"

namespace LinphoneWeb {

FB_FORWARD_PTR(CoreAPI)
FB_FORWARD_PTR(ErrorInfoAPI)

FB_FORWARD_PTR(ProxyConfigAPI)
class ProxyConfigAPI: public WrapperAPI {
	friend class FactoryAPI;
private:
	LinphoneProxyConfig *mProxyConfig;

	ProxyConfigAPI(LinphoneProxyConfig *proxyConfig);
	ProxyConfigAPI();
	
protected:
	virtual void initProxy();
	
public:
	virtual ~ProxyConfigAPI();

	int getAvpfMode() const;
	void setAvpfMode(int mode);
	int getAvpfRrInterval() const;
	void setAvpfRrInterval(int interval);

	CoreAPIPtr getCore() const;

	StringPtr getContactParameters() const;
	void setContactParameters(StringPtr const &parameter);

	bool getDialEscapePlus() const;
	void setDialEscapePlus(bool escape);

	StringPtr getDialPrefix() const;
	void setDialPrefix(StringPtr const &prefix);

	StringPtr getDomain() const;
	int getError() const;
	ErrorInfoAPIPtr getErrorInfo() const;

	bool publishEnabled() const;
	void enablePublish(bool enable);

	StringPtr getSipSetup() const;
	void setSipSetup(StringPtr const &sip_setup);

	void setServerAddr(StringPtr const &server_addr);
	StringPtr getServerAddr() const;

	void setIdentity(StringPtr const &identity);
	StringPtr getIdentity() const;

	void setRoute(StringPtr const &route);
	StringPtr getRoute() const;

	void setExpires(int expires);
	int getExpires() const;
	void setPublishExpires(int expires);
	int getPublishExpires() const;

	void enableRegister(bool val);
	bool registerEnabled() const;

	int getState() const;

	void edit();
	int done();

	StringPtr normalizeNumber(StringPtr const &username) const;
	void refreshRegister();

	inline LinphoneProxyConfig *getRef() {
		return mProxyConfig;
	}
};
	
} // LinphoneWeb

#endif //H_PROXYCONFIGAPI
