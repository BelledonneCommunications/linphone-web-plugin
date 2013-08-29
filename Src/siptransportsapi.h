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

#ifndef H_SIPTRANSPORTSAPI
#define H_SIPTRANSPORTSAPI

#include <linphone/linphonecore.h>
#include "wrapperapi.h"

namespace LinphoneWeb {

FB_FORWARD_PTR(SipTransportsAPI)
class SipTransportsAPI: public WrapperAPI {
	friend class FactoryAPI;
private:
	LCSipTransports mSipTransports;
	
	SipTransportsAPI();
	
protected:
	virtual void initProxy();
	
public:
	virtual ~SipTransportsAPI();
	
	inline const LCSipTransports *getRef() const {
		return &mSipTransports;
	}
	
	inline LCSipTransports *getRef() {
		return &mSipTransports;
	}
	
	void setUdpPort(int port);
	int getUdpPort() const;
	void setTcpPort(int port);
	int getTcpPort() const;
	void setDtlsPort(int port);
	int getDtlsPort() const;
	void setTlsPort(int port);
	int getTlsPort() const;
};

} // LinphoneWeb

#endif //H_SIPTRANSPORTSAPI