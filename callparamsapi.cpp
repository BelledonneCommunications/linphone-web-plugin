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

#include "callparamsapi.h"
#include "utils.h"

CallParamsAPI::CallParamsAPI(LinphoneCallParams *callParams) :
		JSAPIAuto(APIDescription(this)), mCallParams(callParams), mUsed(true), mConst(false) {
	FBLOG_DEBUG("CallParamsAPI::CallParamsAPI", "this=" << this << "\t" << "callParams=" << callParams);
	initProxy();
}

CallParamsAPI::CallParamsAPI(const LinphoneCallParams *callParams) :
		JSAPIAuto(APIDescription(this)), mCallParams(const_cast<LinphoneCallParams *>(callParams)), mUsed(true), mConst(true) {
	FBLOG_DEBUG("CallParamsAPI::CallParamsAPI", "this=" << this << "\t" << "callParams=" << callParams);
	initProxy();
}

void CallParamsAPI::initProxy() {
	registerMethod("copy", make_method(this, &CallParamsAPI::copy));
	registerProperty("localConferenceMode", make_property(this, &CallParamsAPI::localConferenceMode));

	if (mConst) {
		registerMethod("setAudioBandwidthLimit", make_method(this, &CallParamsAPI::setAudioBandwidthLimit));
		registerProperty("earlyMediaSendingEnabled", make_property(this, &CallParamsAPI::earlyMediaSendingEnabled, &CallParamsAPI::enableEarlyMediaSending));
		registerProperty("videoEnabled", make_property(this, &CallParamsAPI::videoEnabled, &CallParamsAPI::enableVideo));
	} else {
		registerProperty("earlyMediaSendingEnabled", make_property(this, &CallParamsAPI::earlyMediaSendingEnabled));
		registerProperty("videoEnabled", make_property(this, &CallParamsAPI::videoEnabled));
	}
}

void CallParamsAPI::setAudioBandwidthLimit(int bw) {
	FBLOG_DEBUG("CallParamsAPI::setAudioBandwidthLimit", "this=" << this << "\t" << "bw=" << bw);
	linphone_call_params_set_audio_bandwidth_limit(mCallParams, bw);
}

bool CallParamsAPI::earlyMediaSendingEnabled() const {
	FBLOG_DEBUG("CallParamsAPI::earlyMediaSendingEnabled", "this=" << this);
	return linphone_call_params_early_media_sending_enabled(mCallParams) == TRUE ? true : false;
}

void CallParamsAPI::enableEarlyMediaSending(bool enable) {
	FBLOG_DEBUG("CallParamsAPI::enableEarlyMediaSending", "this=" << this << "\t" << "enable=" << enable);
	return linphone_call_params_enable_early_media_sending(mCallParams, enable ? TRUE : FALSE);
}

bool CallParamsAPI::localConferenceMode() const {
	FBLOG_DEBUG("CallParamsAPI::localConferenceMode", "this=" << this);
	return linphone_call_params_local_conference_mode(mCallParams) == TRUE ? true : false;
}

//getMediaEncryption	mediaEncryption
//setMediaEncryption	mediaEncryption
//getUsedAudioCodec	usedAudioCodec
//getUsedVideoCodec	usedVideoCodec
void CallParamsAPI::enableVideo(bool enable) {
	FBLOG_DEBUG("CallParamsAPI::enableVideo", "this=" << this << "\t" << "enable=" << enable);
	return linphone_call_params_enable_video(mCallParams, enable ? TRUE : FALSE);

}

bool CallParamsAPI::videoEnabled() const {
	FBLOG_DEBUG("CallParamsAPI::videoEnabled", "this=" << this);
	return linphone_call_params_video_enabled(mCallParams) == TRUE ? true : false;

}

CallParamsAPIPtr CallParamsAPI::copy() const {
	FBLOG_DEBUG("CallParamsAPI::getRefKey", "this=" << this);
	CallParamsAPIPtr ret(new CallParamsAPI(linphone_call_params_copy(mCallParams)));
	ret->mUsed = false;
	return ret;
}

CallParamsAPI::~CallParamsAPI() {
	FBLOG_DEBUG("CallParamsAPI::~CallParamsAPI", "this=" << this);
}

CallParamsAPIPtr CallParamsAPI::get(LinphoneCallParams *callParams) {
	if (callParams == NULL)
		return CallParamsAPIPtr();

	CallParamsAPIPtr shared_ptr;
	shared_ptr = CallParamsAPIPtr(new CallParamsAPI(callParams));
	return shared_ptr;
}

CallParamsAPIPtr CallParamsAPI::get(const LinphoneCallParams *callParams) {
	if (callParams == NULL)
		return CallParamsAPIPtr();

	CallParamsAPIPtr shared_ptr;
	shared_ptr = CallParamsAPIPtr(new CallParamsAPI(callParams));
	return shared_ptr;
}
