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

#ifndef H_TUNNELCONFIGAPI
#define H_TUNNELCONFIGAPI

#include <linphone/linphonecore.h>
#include <linphone/linphone_tunnel.h>
#include "wrapperapi.h"

namespace LinphoneWeb {

FB_FORWARD_PTR(CoreAPI)

FB_FORWARD_PTR(TunnelConfigAPI)
class TunnelConfigAPI: public WrapperAPI {
	friend class FactoryAPI;
private:
	LinphoneTunnelConfig *mTunnelConfig;

	TunnelConfigAPI(LinphoneTunnelConfig *tunnelConfig);
	TunnelConfigAPI();
	
protected:
	virtual void initProxy();

public:
	virtual ~TunnelConfigAPI();

	int getDelay() const;
	void setDelay(int delay);
	StringPtr getHost() const;
	void setHost(StringPtr const &host);
	int getPort() const;
	void setPort(int port);
	int getRemoteUdpMirrorPort() const;
	void setRemoteUdpMirrorPort(int port);

	inline LinphoneTunnelConfig *getRef() const {
		return mTunnelConfig;
	}
};
	
} // LinphoneWeb

#endif //H_TUNNELCONFIGAPI
