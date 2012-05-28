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

#ifndef H_CALLPARAMSAPI
#define H_CALLPARAMSAPI

#include <JSAPIAuto.h>
#include <linphonecore.h>

FB_FORWARD_PTR(CallParamsAPI)
class CallParamsAPI: public FB::JSAPIAuto {
private:
	LinphoneCallParams *mCallParams;
	bool mUsed;
	bool mConst;

	CallParamsAPI(LinphoneCallParams *callParams);
	CallParamsAPI(const LinphoneCallParams *callParams);

	void initProxy();
public:
	~CallParamsAPI();

	void setAudioBandwidthLimit(int bw);
	bool earlyMediaSendingEnabled() const;
	void enableEarlyMediaSending(bool enable);
	bool localConferenceMode() const;
	//getMediaEncryption	mediaEncryption
	//setMediaEncryption	mediaEncryption
	//getUsedAudioCodec	usedAudioCodec
	//getUsedVideoCodec	usedVideoCodec
	void enableVideo(bool enable);
	bool videoEnabled() const;
	CallParamsAPIPtr copy() const;


	inline LinphoneCallParams *getRef() {
		mUsed = true;
		return mCallParams;
	}
	static CallParamsAPIPtr get(LinphoneCallParams *callParams);
	static CallParamsAPIPtr get(const LinphoneCallParams *callParams);
};

#endif //H_CALLPARAMSAPI
