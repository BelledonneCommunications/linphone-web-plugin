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

#ifndef H_CALLAPI
#define H_CALLAPI

#include <linphone/linphonecore.h>
#include "wrapperapi.h"

namespace LinphoneWeb {

FB_FORWARD_PTR(CallLogAPI)
FB_FORWARD_PTR(CallParamsAPI)
FB_FORWARD_PTR(CallStatsAPI)
FB_FORWARD_PTR(AddressAPI)
FB_FORWARD_PTR(CoreAPI)
FB_FORWARD_PTR(ErrorInfoAPI)
FB_FORWARD_PTR(VideoAPI)

FB_FORWARD_PTR(CallAPI)
class CallAPI: public WrapperAPI {
	friend class FactoryAPI;
private:
	LinphoneCall *mCall;

	CallAPI(LinphoneCall *call);
	
protected:
	virtual void initProxy();
	
public:
	virtual ~CallAPI();
	
	CallStatsAPIPtr getAudioStats() const;
	StringPtr getAuthenticationToken() const;
	bool getAuthenticationTokenVerified() const;
	float getAverageQuality() const;
	CallLogAPIPtr getCallLog() const;
	CoreAPIPtr getCore() const;
	CallParamsAPIPtr getCurrentParams() const;
	float getCurrentQuality() const;
	int getDir() const;
	int getDuration() const;
	ErrorInfoAPIPtr getErrorInfo() const;
	float getPlayVolume() const;
	int getReason() const;
	float getRecordVolume() const;
	StringPtr getReferTo() const;
	AddressAPIPtr getRemoteAddress() const;
	StringPtr getRemoteAddressAsString() const;
	StringPtr getRemoteContact() const;
	CallParamsAPIPtr getRemoteParams() const;
	StringPtr getRemoteUserAgent() const;
	CallAPIPtr getReplacedCall() const;
	LinphoneCallState getState() const;
	LinphoneCallState getTransferState() const;
	CallStatsAPIPtr getVideoStats() const;

	bool cameraEnabled() const;
	void enableCamera(bool enabled);
	bool echoCancellationEnabled() const;
	void enableEchoCancellation(bool enabled);
	bool echoLimiterEnabled() const;
	void enableEchoLimiter(bool enabled);
	void setNativeVideoWindowId(WhiteBoard::IdType id);
	WhiteBoard::IdType getNativeVideoWindowId() const;

	bool askedToAutoanswer() const;
	bool hasTransferPending() const;
	void sendVfuRequest();
	void setAuthenticationTokenVerified(bool verified);
	void zoomVideo(float zoom, float cx, float cy);
	
	void startRecording();
	void stopRecording();

	inline LinphoneCall *getRef() const {
		return mCall;
	}
	
private:
	VideoAPIWeakPtr mVideoWindow;
	static void videoWindowEventHandler(const CallAPIWeakPtr &callPtr, void *ptr);
	void setVideoWindow(void *ptr);
};
	
} // LinphoneWeb

#endif //H_CALLAPI
