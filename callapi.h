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

#ifndef H_CALLAPI
#define H_CALLAPI

#include <JSAPIAuto.h>
#include <linphonecore.h>
#include "wrapperapi.h"

FB_FORWARD_PTR(CallLogAPI)
FB_FORWARD_PTR(CallParamsAPI)
FB_FORWARD_PTR(CallStatsAPI)
FB_FORWARD_PTR(AddressAPI)
FB_FORWARD_PTR(CoreAPI)

FB_FORWARD_PTR(CallAPI)
class CallAPI: public FB::JSAPIAuto, public WrapperAPI {
    friend class FactoryAPI;
private:
	LinphoneCall *mCall;

	CallAPI(LinphoneCall *call);
    
protected:
	void initProxy();
    
public:
	~CallAPI();
	CallStatsAPIPtr getAudioStats() const;
	std::string getAuthenticationToken() const;
	bool getAuthenticationTokenVerified() const;
	float getAverageQuality() const;
	CallLogAPIPtr getCallLog() const;
	CoreAPIPtr getCore() const;
	CallParamsAPIPtr getCurrentParams() const;
	float getCurrentQuality() const;
	int getDir() const;
	int getDuration() const;
	float getPlayVolume() const;
	int getReason() const;
	float getRecordVolume() const;
	std::string getReferTo() const;
	AddressAPIPtr getRemoteAddress() const;
	std::string getRemoteAddressAsString() const;
	CallParamsAPIPtr getRemoteParams() const;
	std::string getRemoteUserAgent() const;
	CallAPIPtr getReplacedCall() const;
	int getState() const;
	int getTransferState() const;
	CallStatsAPIPtr getVideoStats() const;
	bool inConference() const;

	bool cameraEnabled() const;
	void enableCamera(bool enabled);
	bool echoCancellationEnabled() const;
	void enableEchoCancellation(bool enabled);
	bool echoLimiterEnabled() const;
	void enableEchoLimiter(bool enabled);

	bool askedToAutoanswer() const;
	bool hasTransferPending() const;
	void sendVfuRequest();
	void setAuthenticationTokenVerified(bool verified);
	void zoomVideo(float zoom, float cx, float cy);

	inline LinphoneCall *getRef() const {
		return mCall;
	}
};

#endif //H_CALLAPI
