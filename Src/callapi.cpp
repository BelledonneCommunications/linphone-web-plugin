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

#include "addressapi.h"
#include "callapi.h"
#include "calllogapi.h"
#include "callparamsapi.h"
#include "callstatsapi.h"
#include "coreapi.h"

#include "utils.h"
#include "factoryapi.h"

namespace LinphoneWeb {

CallAPI::CallAPI(LinphoneCall *call) :
		WrapperAPI(APIDescription(this)), mCall(call) {
	FBLOG_DEBUG("CallAPI::CallAPI", "this=" << this << "\t" << "call=" << call);
	linphone_call_ref(mCall);
	linphone_call_set_user_pointer(mCall, this);
}

void CallAPI::initProxy() {
	registerProperty("audioStats", FB::make_property(this, &CallAPI::getAudioStats));
	registerProperty("authenticationToken", FB::make_property(this, &CallAPI::getAuthenticationToken));
	registerProperty("averageQuality", FB::make_property(this, &CallAPI::getAverageQuality));
	registerProperty("cameraEnabled", FB::make_property(this, &CallAPI::cameraEnabled, &CallAPI::enableCamera));
	registerProperty("callLog", FB::make_property(this, &CallAPI::getCallLog));
	registerProperty("core", FB::make_property(this, &CallAPI::getCore));
	registerProperty("currentParams", FB::make_property(this, &CallAPI::getCurrentParams));
	registerProperty("currentQuality", FB::make_property(this, &CallAPI::getCurrentQuality));
	registerProperty("dir", FB::make_property(this, &CallAPI::getDir));
	registerProperty("duration", FB::make_property(this, &CallAPI::getDuration));
	registerProperty("echoCancellationEnabled", FB::make_property(this, &CallAPI::echoCancellationEnabled, &CallAPI::enableEchoCancellation));
	registerProperty("echoLimiterEnabled", FB::make_property(this, &CallAPI::echoLimiterEnabled, &CallAPI::enableEchoLimiter));
	registerProperty("errorInfo", FB::make_property(this, &CallAPI::getErrorInfo));
	registerProperty("nativeVideoWindowId", make_property(this, &CallAPI::getNativeVideoWindowId, &CallAPI::setNativeVideoWindowId));
	registerProperty("playVolume", FB::make_property(this, &CallAPI::getPlayVolume));
	registerProperty("reason", FB::make_property(this, &CallAPI::getReason));
	registerProperty("recordVolume", FB::make_property(this, &CallAPI::getRecordVolume));
	registerProperty("referTo", FB::make_property(this, &CallAPI::getReferTo));
	registerProperty("remoteAddress", FB::make_property(this, &CallAPI::getRemoteAddress));
	registerProperty("remoteAddressAsString", FB::make_property(this, &CallAPI::getRemoteAddressAsString));
	registerProperty("remoteContact", FB::make_property(this, &CallAPI::getRemoteContact));
	registerProperty("remoteParams", FB::make_property(this, &CallAPI::getRemoteParams));
	registerProperty("remoteUserAgent", FB::make_property(this, &CallAPI::getRemoteUserAgent));
	registerProperty("replacedCall", FB::make_property(this, &CallAPI::getReplacedCall));
	registerProperty("state", FB::make_property(this, &CallAPI::getState));
	registerProperty("transferState", FB::make_property(this, &CallAPI::getTransferState));
	registerProperty("videoStats", FB::make_property(this, &CallAPI::getVideoStats));

	registerMethod("askedToAutoanswer", make_method(this, &CallAPI::askedToAutoanswer));
	registerMethod("hasTransferPending", make_method(this, &CallAPI::hasTransferPending));
	registerMethod("sendVfuRequest", make_method(this, &CallAPI::sendVfuRequest));
	registerMethod("setAuthenticationTokenVerified", make_method(this, &CallAPI::setAuthenticationTokenVerified));
	registerMethod("zoomVideo", make_method(this, &CallAPI::zoomVideo));
	
	registerMethod("startRecording", make_method(this, &CallAPI::startRecording));
	registerMethod("stopRecording", make_method(this, &CallAPI::stopRecording));
}

CallAPI::~CallAPI() {
	FBLOG_DEBUG("CallAPI::~CallAPI", "this=" << this);
	if(mCall) {
		linphone_call_set_user_pointer(mCall, NULL);
		linphone_call_unref(mCall);
	}
}

CallStatsAPIPtr CallAPI::getAudioStats() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallAPI::getAudioStats", "this=" << this);
	return getFactory()->getCallStats(linphone_call_get_audio_stats(mCall));
}

StringPtr CallAPI::getAuthenticationToken() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::getAuthenticationToken", "this=" << this);
	return CHARPTR_TO_STRING(linphone_call_get_authentication_token(mCall));
}

bool CallAPI::getAuthenticationTokenVerified() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::getAuthenticationTokenVerified", "this=" << this);
	return linphone_call_get_authentication_token_verified(mCall) == TRUE? true : false;
}

float CallAPI::getAverageQuality() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::getCurrentQuality", "this=" << this);
	return linphone_call_get_average_quality(mCall);
}

CallLogAPIPtr CallAPI::getCallLog() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::getCallLog", "this=" << this);
	return getFactory()->getCallLog(linphone_call_get_call_log(mCall));
}

CoreAPIPtr CallAPI::getCore() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::getCurrentQuality", "this=" << this);
	return getFactory()->getCore(linphone_call_get_core(mCall));
}

CallParamsAPIPtr CallAPI::getCurrentParams() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::getCurrentParams", "this=" << this);
	const LinphoneCallParams *const_params = linphone_call_get_current_params(mCall);
	LinphoneCallParams *params = linphone_call_params_copy(const_params);
	return getFactory()->getCallParams(params);
}

float CallAPI::getCurrentQuality() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::getCurrentQuality", "this=" << this);
	return linphone_call_get_current_quality(mCall);
}

int CallAPI::getDir() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::getDir", "this=" << this);
	return linphone_call_get_dir(mCall);
}

int CallAPI::getDuration() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::getDir", "this=" << this);
	return linphone_call_get_duration(mCall);
}

ErrorInfoAPIPtr CallAPI::getErrorInfo() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::getErrorInfo", "this=" << this);
	return getFactory()->getErrorInfo(linphone_call_get_error_info(mCall));
}

void CallAPI::videoWindowEventHandler(const CallAPIWeakPtr &callPtr, void *ptr) {
	CallAPIPtr call = callPtr.lock();
	if (call) {
		call->setVideoWindow(ptr);
	}
}
	
void CallAPI::setVideoWindow(void *ptr) {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::setVideoWindow", "this=" << this << "\t" << "ptr=" << ptr);
	if (ptr != NULL) {
		linphone_call_set_native_video_window_id(mCall, ptr);
	} else {
		linphone_call_set_native_video_window_id(mCall, (void *)(unsigned long)-1);
	}
}

void CallAPI::setNativeVideoWindowId(WhiteBoard::IdType id) {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::setNativeVideoWindowId", "this=" << this << "\t" << "id=" << id);
	
	if(getFactory()->getWhiteBoard()->getId(mVideoWindow) == id) {
		// Same do nothing
		return;
	}

	mVideoWindow = getFactory()->getWhiteBoard()->getValue<VideoAPIWeakPtr>(id);
	VideoAPIPtr video = mVideoWindow.lock();
	if(video) {
		video->setWindowEventHandler(boost::bind(&CallAPI::videoWindowEventHandler,
			CallAPIWeakPtr(boost::static_pointer_cast<CallAPI>(this->shared_from_this())), _1));
	}
}

WhiteBoard::IdType CallAPI::getNativeVideoWindowId() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::getNativeVideoWindowId", "this=" << this);
	
	return getFactory()->getWhiteBoard()->getId(mVideoWindow);
}

float CallAPI::getPlayVolume() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::getPlayVolume", "this=" << this);
	return linphone_call_get_play_volume(mCall);
}

int CallAPI::getReason() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::getReason", "this=" << this);
	return linphone_call_get_reason(mCall);
}

float CallAPI::getRecordVolume() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::getReason", "this=" << this);
	return linphone_call_get_record_volume(mCall);
}

StringPtr CallAPI::getReferTo() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::getReason", "this=" << this);
	return CHARPTR_TO_STRING(linphone_call_get_refer_to(mCall));
}

AddressAPIPtr CallAPI::getRemoteAddress() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::getRemoteAddress", "this=" << this);
	return getFactory()->getAddress(linphone_call_get_remote_address(mCall));
}

StringPtr CallAPI::getRemoteAddressAsString() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::getRemoteAddressAsString", "this=" << this);
	return CHARPTR_TO_STRING(linphone_call_get_remote_address_as_string(mCall));
}

StringPtr CallAPI::getRemoteContact() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallAPI::getRemoteContact", "this=" << this);
	return CHARPTR_TO_STRING(linphone_call_get_remote_contact(mCall));
}

CallParamsAPIPtr CallAPI::getRemoteParams() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::getRemoteParams", "this=" << this);
	return getFactory()->getCallParams(linphone_call_get_remote_params(mCall));
}

StringPtr CallAPI::getRemoteUserAgent() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::getRemoteUserAgent", "this=" << this);
	return CHARPTR_TO_STRING(linphone_call_get_remote_user_agent(mCall));
}

CallAPIPtr CallAPI::getReplacedCall() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::getReplacedCall", "this=" << this);
	return getFactory()->getCall(linphone_call_get_replaced_call(mCall));
}

LinphoneCallState CallAPI::getState() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::getState", "this=" << this);
	return linphone_call_get_state(mCall);
}

LinphoneCallState CallAPI::getTransferState() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::getTransferState", "this=" << this);
	return linphone_call_get_transfer_state(mCall);
}

CallStatsAPIPtr CallAPI::getVideoStats() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::getVideoStats", "this=" << this);
	return getFactory()->getCallStats(linphone_call_get_video_stats(mCall));
}

bool CallAPI::cameraEnabled() const {
	FBLOG_DEBUG("CallAPI::cameraEnabled", "this=" << this);
	return linphone_call_camera_enabled(mCall) == TRUE ? true : false;
}

void CallAPI::enableCamera(bool enabled) {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::enableCamera", "this=" << this << "\t" << "enabled=" << enabled);
	linphone_call_enable_camera(mCall, enabled ? TRUE : FALSE);
}

bool CallAPI::echoCancellationEnabled() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::echoCancellationEnabled", "this=" << this);
	return linphone_call_echo_cancellation_enabled(mCall) == TRUE ? true : false;
}

void CallAPI::enableEchoCancellation(bool enabled) {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::enableEchoCancellation", "this=" << this << "\t" << "enabled=" << enabled);
	linphone_call_enable_echo_cancellation(mCall, enabled ? TRUE : FALSE);
}

bool CallAPI::echoLimiterEnabled() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::echoLimiterEnabled", "this=" << this);
	return linphone_call_echo_limiter_enabled(mCall) == TRUE ? true : false;
}

void CallAPI::enableEchoLimiter(bool enabled) {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::enableEchoLimiter", "this=" << this << "\t" << "enabled=" << enabled);
	linphone_call_enable_echo_limiter(mCall, enabled ? TRUE : FALSE);
}

bool CallAPI::askedToAutoanswer() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::askedToAutoanswer", "this=" << this);
	return linphone_call_asked_to_autoanswer(mCall) == TRUE ? true : false;
}

bool CallAPI::hasTransferPending() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::hasTransferPending", "this=" << this);
	return linphone_call_has_transfer_pending(mCall) == TRUE ? true : false;
}

void CallAPI::sendVfuRequest() {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::sendVfuRequest", "this=" << this);
	linphone_call_send_vfu_request(mCall);
}

void CallAPI::setAuthenticationTokenVerified(bool verified) {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::setAuthenticationTokenVerified", "this=" << this);
	linphone_call_set_authentication_token_verified(mCall, verified);
}

void CallAPI::zoomVideo(float zoom, float cx, float cy) {
	CORE_MUTEX

	FBLOG_DEBUG("CallAPI::zoomVideo", "this=" << this << "\t" << "zoom=" << zoom << ", cx=" << cx << ", cy=" << cy);
	linphone_call_zoom_video(mCall, zoom, &cx, &cy);
}

void CallAPI::startRecording() {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallAPI::startRecording", "this=" << this);
	linphone_call_start_recording(mCall);

}

void CallAPI::stopRecording() {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallAPI::stopRecording", "this=" << this);
	linphone_call_stop_recording(mCall);
}
	
} // LinphoneWeb
