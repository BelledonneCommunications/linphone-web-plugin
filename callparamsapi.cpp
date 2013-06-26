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
#include "factoryapi.h"

namespace LinphoneWeb {

CallParamsAPI::CallParamsAPI(LinphoneCallParams *callParams) :
		WrapperAPI(APIDescription(this)), mCallParams(callParams) {
	FBLOG_DEBUG("CallParamsAPI::CallParamsAPI", "this=" << this << "\t" << "callParams=" << callParams);
}

CallParamsAPI::CallParamsAPI(const LinphoneCallParams *callParams) :
		WrapperAPI(APIDescription(this)), mCallParams(const_cast<LinphoneCallParams *>(callParams)) {
	FBLOG_DEBUG("CallParamsAPI::CallParamsAPI", "this=" << this << "\t" << "callParams=" << callParams);
}

CallParamsAPI::~CallParamsAPI() {
	FBLOG_DEBUG("CallParamsAPI::~CallParamsAPI", "this=" << this);
}
	
void CallParamsAPI::initProxy() {
	registerMethod("copy", make_method(this, &CallParamsAPI::copy));
	registerProperty("localConferenceMode", make_property(this, &CallParamsAPI::localConferenceMode));

	if (!isConst()) {
		registerProperty("audioBandwidthLimit", make_property(this, &CallParamsAPI::getAudioBandwidthLimit, &CallParamsAPI::setAudioBandwidthLimit));
		registerProperty("earlyMediaSendingEnabled", make_property(this, &CallParamsAPI::earlyMediaSendingEnabled, &CallParamsAPI::enableEarlyMediaSending));
		registerProperty("lowBandwidthEnabled", make_property(this, &CallParamsAPI::lowBandwidthEnabled, &CallParamsAPI::enableLowBandwidth));
		registerProperty("recordFile", make_property(this, &CallParamsAPI::getRecordFile, &CallParamsAPI::setRecordFile));
		registerProperty("videoEnabled", make_property(this, &CallParamsAPI::videoEnabled, &CallParamsAPI::enableVideo));
	} else {
		registerProperty("audioBandwidthLimit", make_property(this, &CallParamsAPI::getAudioBandwidthLimit));
		registerProperty("earlyMediaSendingEnabled", make_property(this, &CallParamsAPI::earlyMediaSendingEnabled));
		registerProperty("lowBandwidthEnabled", make_property(this, &CallParamsAPI::lowBandwidthEnabled));
		registerProperty("recordFile", make_property(this, &CallParamsAPI::getRecordFile));
		registerProperty("videoEnabled", make_property(this, &CallParamsAPI::videoEnabled));
	}
}

int CallParamsAPI::getAudioBandwidthLimit() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallParamsAPI::getAudioBandwidthLimit", "this=" << this);
	FBLOG_ERROR("CallParamsAPI::getAudioBandwidthLimit", "NOT IMPLEMENTED");
	// TODO Stub
	//return linphone_call_params_get_audio_bandwidth_limit(mCallParams);
	return -1;
}


void CallParamsAPI::setAudioBandwidthLimit(int bw) {
	CORE_MUTEX

	FBLOG_DEBUG("CallParamsAPI::setAudioBandwidthLimit", "this=" << this << "\t" << "bw=" << bw);
	linphone_call_params_set_audio_bandwidth_limit(mCallParams, bw);
}

bool CallParamsAPI::earlyMediaSendingEnabled() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallParamsAPI::earlyMediaSendingEnabled", "this=" << this);
	return linphone_call_params_early_media_sending_enabled(mCallParams) == TRUE ? true : false;
}

void CallParamsAPI::enableEarlyMediaSending(bool enable) {
	CORE_MUTEX

	FBLOG_DEBUG("CallParamsAPI::enableEarlyMediaSending", "this=" << this << "\t" << "enable=" << enable);
	return linphone_call_params_enable_early_media_sending(mCallParams, enable ? TRUE : FALSE);
}

bool CallParamsAPI::localConferenceMode() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallParamsAPI::localConferenceMode", "this=" << this);
	return linphone_call_params_local_conference_mode(mCallParams) == TRUE ? true : false;
}

bool CallParamsAPI::lowBandwidthEnabled() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallParamsAPI::lowBandwidthEnabled", "this=" << this);
	return linphone_call_params_low_bandwidth_enabled(mCallParams) == TRUE ? true : false;
}

void CallParamsAPI::enableLowBandwidth(bool enable) {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallParamsAPI::enableLowBandwidth", "this=" << this << "\t" << "enable=" << enable);
	linphone_call_params_enable_low_bandwidth(mCallParams, enable ? TRUE : FALSE);
}

//getMediaEncryption	mediaEncryption
//setMediaEncryption	mediaEncryption
//getUsedAudioCodec	usedAudioCodec
//getUsedVideoCodec	usedVideoCodec

StringPtr CallParamsAPI::getRecordFile() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallParamsAPI::getRecordFile", "this=" << this);
	return CHARPTR_TO_STRING(linphone_call_params_get_record_file(mCallParams));
}

void CallParamsAPI::setRecordFile(const StringPtr &file) {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallParamsAPI::setRecordFile", "this=" << this << "\t" << "file=" << file);
	linphone_call_params_set_record_file(mCallParams, STRING_TO_CHARPTR(file));
}

void CallParamsAPI::enableVideo(bool enable) {
	CORE_MUTEX

	FBLOG_DEBUG("CallParamsAPI::enableVideo", "this=" << this << "\t" << "enable=" << enable);
	return linphone_call_params_enable_video(mCallParams, enable ? TRUE : FALSE);

}

bool CallParamsAPI::videoEnabled() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallParamsAPI::videoEnabled", "this=" << this);
	return linphone_call_params_video_enabled(mCallParams) == TRUE ? true : false;

}

CallParamsAPIPtr CallParamsAPI::copy() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallParamsAPI::getRefKey", "this=" << this);
	CallParamsAPIPtr ret(new CallParamsAPI(linphone_call_params_copy(mCallParams)));
	ret->own();
	return ret;
}
	
} // LinphoneWeb
