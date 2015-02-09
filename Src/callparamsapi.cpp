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
	registerMethod("getCustomHeader", make_method(this, &CallParamsAPI::getCustomHeader));
	registerProperty("localConferenceMode", make_property(this, &CallParamsAPI::localConferenceMode));
	registerProperty("receivedVideoSize", make_property(this, &CallParamsAPI::getReceivedVideoSize));
	registerProperty("sentVideoSize", make_property(this, &CallParamsAPI::getSentVideoSize));
	registerProperty("usedAudioCodec", make_property(this, &CallParamsAPI::getUsedAudioCodec));
	registerProperty("usedVideoCodec", make_property(this, &CallParamsAPI::getUsedVideoCodec));

	if (!isConst()) {
		registerMethod("addCustomHeader", make_method(this, &CallParamsAPI::addCustomHeader));
		registerProperty("audioBandwidthLimit", make_property(this, &CallParamsAPI::getAudioBandwidthLimit, &CallParamsAPI::setAudioBandwidthLimit));
		registerProperty("audioDirection", make_property(this, &CallParamsAPI::getAudioDirection, &CallParamsAPI::setAudioDirection));
		registerProperty("earlyMediaSendingEnabled", make_property(this, &CallParamsAPI::earlyMediaSendingEnabled, &CallParamsAPI::enableEarlyMediaSending));
		registerProperty("lowBandwidthEnabled", make_property(this, &CallParamsAPI::lowBandwidthEnabled, &CallParamsAPI::enableLowBandwidth));
		registerProperty("mediaEncryption", make_property(this, &CallParamsAPI::getMediaEncryption, &CallParamsAPI::setMediaEncryption));
		registerProperty("recordFile", make_property(this, &CallParamsAPI::getRecordFile, &CallParamsAPI::setRecordFile));
		registerProperty("videoDirection", make_property(this, &CallParamsAPI::getVideoDirection, &CallParamsAPI::setVideoDirection));
		registerProperty("videoEnabled", make_property(this, &CallParamsAPI::videoEnabled, &CallParamsAPI::enableVideo));
	} else {
		registerProperty("audioBandwidthLimit", make_property(this, &CallParamsAPI::getAudioBandwidthLimit));
		registerProperty("audioDirection", make_property(this, &CallParamsAPI::getAudioDirection));
		registerProperty("earlyMediaSendingEnabled", make_property(this, &CallParamsAPI::earlyMediaSendingEnabled));
		registerProperty("lowBandwidthEnabled", make_property(this, &CallParamsAPI::lowBandwidthEnabled));
		registerProperty("mediaEncryption", make_property(this, &CallParamsAPI::getMediaEncryption));
		registerProperty("recordFile", make_property(this, &CallParamsAPI::getRecordFile));
		registerProperty("videoDirection", make_property(this, &CallParamsAPI::getVideoDirection));
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

int CallParamsAPI::getAudioDirection() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallParamsAPI::getAudioDirection", "this=" << this);
	return (int)linphone_call_params_get_audio_direction(mCallParams);
}

void CallParamsAPI::setAudioDirection(int dir) {
	CORE_MUTEX

	FBLOG_DEBUG("CallParamsAPI::setAudioDirection", "this=" << this << "\t" << "dir=" << dir);
	linphone_call_params_set_audio_direction(mCallParams, (LinphoneMediaDirection)dir);
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

int CallParamsAPI::getMediaEncryption() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallParamsAPI::getMediaEncryption", "this=" << this);
	return linphone_call_params_get_media_encryption(mCallParams);
}

void CallParamsAPI::setMediaEncryption(int encryption) {
	CORE_MUTEX

	FBLOG_DEBUG("CallParamsAPI::setMediaEncryption", "this=" << this << "\t" << "encryption=" << encryption);
	linphone_call_params_set_media_encryption(mCallParams, (LinphoneMediaEncryption)encryption);
}

MSVideoSizeAPIPtr CallParamsAPI::getReceivedVideoSize() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallParamsAPI::getReceivedVideoSize", "this=" << this);
	return getFactory()->getMSVideoSize(linphone_call_params_get_received_video_size(mCallParams));
}

MSVideoSizeAPIPtr CallParamsAPI::getSentVideoSize() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallParamsAPI::getSentVideoSize", "this=" << this);
	return getFactory()->getMSVideoSize(linphone_call_params_get_sent_video_size(mCallParams));
}

PayloadTypeAPIPtr CallParamsAPI::getUsedAudioCodec() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallParamsAPI::getUsedAudioCodec", "this=" << this);
	return getFactory()->getPayloadType(linphone_call_params_get_used_audio_codec(mCallParams));
}

PayloadTypeAPIPtr CallParamsAPI::getUsedVideoCodec() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallParamsAPI::getUsedVideoCodec", "this=" << this);
	return getFactory()->getPayloadType(linphone_call_params_get_used_video_codec(mCallParams));
}

StringPtr CallParamsAPI::getRecordFile() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallParamsAPI::getRecordFile", "this=" << this);
	return CHARPTR_TO_STRING(linphone_call_params_get_record_file(mCallParams));
}

void CallParamsAPI::setRecordFile(StringPtr const &file) {
	CORE_MUTEX

	FBLOG_DEBUG("CallParamsAPI::setRecordFile", "this=" << this << "\t" << "file=" << file);
	linphone_call_params_set_record_file(mCallParams, STRING_TO_CHARPTR(file));
}

int CallParamsAPI::getVideoDirection() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallParamsAPI::getVideoDirection", "this=" << this);
	return (int)linphone_call_params_get_video_direction(mCallParams);
}

void CallParamsAPI::setVideoDirection(int dir) {
	CORE_MUTEX

	FBLOG_DEBUG("CallParamsAPI::setVideoDirection", "this=" << this << "\t" << "dir=" << dir);
	linphone_call_params_set_video_direction(mCallParams, (LinphoneMediaDirection)dir);
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

	FBLOG_DEBUG("CallParamsAPI::copy", "this=" << this);
	CallParamsAPIPtr ret(new CallParamsAPI(linphone_call_params_copy(mCallParams)));
	ret->own();
	return ret;
}

void CallParamsAPI::addCustomHeader(StringPtr const &headerName, StringPtr const &headerValue) {
	CORE_MUTEX

	FBLOG_DEBUG("CallParamsAPI::addCustomHeader", "this=" << this << "\n" << "headerName=" << headerName << "\n" << "headerValue=" << headerValue);
	linphone_call_params_add_custom_header(mCallParams, STRING_TO_CHARPTR(headerName), STRING_TO_CHARPTR(headerValue));
}

StringPtr CallParamsAPI::getCustomHeader(StringPtr const &headerName) const {
	CORE_MUTEX

	FBLOG_DEBUG("CallParamsAPI::getCustomHeader", "this=" << this << "\n" << "headerName=" << headerName);
	return CHARPTR_TO_STRING(linphone_call_params_get_custom_header(mCallParams, STRING_TO_CHARPTR(headerName)));
}

} // LinphoneWeb
