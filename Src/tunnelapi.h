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
 */

#ifndef H_TUNNELAPI
#define H_TUNNELAPI

#include <linphone/linphonecore.h>
#include "wrapperapi.h"

namespace LinphoneWeb {

FB_FORWARD_PTR(CoreAPI)

FB_FORWARD_PTR(TunnelAPI)
FB_FORWARD_PTR(TunnelConfigAPI)
class TunnelAPI: public WrapperAPI {
	friend class FactoryAPI;
private:
	LinphoneTunnel *mTunnel;

	TunnelAPI(LinphoneTunnel *tunnel);
	
protected:
	virtual void initProxy();

public:
	virtual ~TunnelAPI();

	bool connected() const;
	int getMode() const;
	void setMode(int mode);
	std::vector<TunnelConfigAPIPtr> getServers() const;
	void enableSip(bool enable);
	bool sipEnabled() const;

	void addServer(TunnelConfigAPIPtr const &server);
	void cleanServers();
	void reconnect();
	void removeServer(TunnelConfigAPIPtr const &server);

	inline LinphoneTunnel *getRef() const {
		return mTunnel;
	}
};
	
} // LinphoneWeb

#endif //H_TUNNELAPI
