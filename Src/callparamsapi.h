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

#ifndef H_CALLPARAMSAPI
#define H_CALLPARAMSAPI

#include <linphone/linphonecore.h>
#include "wrapperapi.h"

namespace LinphoneWeb {

FB_FORWARD_PTR(MSVideoSizeAPI)
FB_FORWARD_PTR(PayloadTypeAPI)
	
FB_FORWARD_PTR(CallParamsAPI)
class CallParamsAPI: public WrapperAPI {
	friend class FactoryAPI;
private:
	LinphoneCallParams *mCallParams;

	CallParamsAPI(LinphoneCallParams *callParams);
	CallParamsAPI(const LinphoneCallParams *callParams);
	
protected:
	virtual void initProxy();
	
public:
	virtual ~CallParamsAPI();

	int getAudioBandwidthLimit() const;
	void setAudioBandwidthLimit(int bw);
	int getAudioDirection() const;
	void setAudioDirection(int dir);
	bool earlyMediaSendingEnabled() const;
	void enableEarlyMediaSending(bool enable);
	bool localConferenceMode() const;
	bool lowBandwidthEnabled() const;
	void enableLowBandwidth(bool enable);
	int getMediaEncryption() const;
	void setMediaEncryption(int encryption);
	MSVideoSizeAPIPtr getReceivedVideoSize() const;
	MSVideoSizeAPIPtr getSentVideoSize() const;
	PayloadTypeAPIPtr getUsedAudioCodec() const;
	PayloadTypeAPIPtr getUsedVideoCodec() const;
	StringPtr getRecordFile() const;
	void setRecordFile(StringPtr const &file);
	int getVideoDirection() const;
	void setVideoDirection(int dir);
	void enableVideo(bool enable);
	bool videoEnabled() const;
	CallParamsAPIPtr copy() const;
	void addCustomHeader(StringPtr const &headerName, StringPtr const &headerValue);
	StringPtr getCustomHeader(StringPtr const &headerName) const;

	inline LinphoneCallParams *getRef() {
		return mCallParams;
	}
};

} // LinphoneWeb

#endif //H_CALLPARAMSAPI
