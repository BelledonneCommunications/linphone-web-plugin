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

#include "addressapi.h"
#include "callapi.h"
#include "calllogapi.h"
#include "callparamsapi.h"
#include "callstatsapi.h"
#include "coreapi.h"

#include "utils.h"
#include "factoryapi.h"

CallAPI::CallAPI(LinphoneCall *call) :
		WrapperAPI(APIDescription(this)), mCall(call) {
    mUsed = true;
    mUsed = true;
	FBLOG_DEBUG("CallAPI::CallAPI", "this=" << this << "\t" << "call=" << call);
	linphone_call_ref(mCall);
	linphone_call_set_user_pointer(mCall, this);
	initProxy();
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
	registerProperty("echoConcellationEnabled", FB::make_property(this, &CallAPI::echoCancellationEnabled, &CallAPI::enableEchoCancellation));
	registerProperty("echoLimiterEnabled", FB::make_property(this, &CallAPI::echoLimiterEnabled, &CallAPI::enableEchoLimiter));
	registerProperty("playVolume", FB::make_property(this, &CallAPI::getPlayVolume));
	registerProperty("reason", FB::make_property(this, &CallAPI::getReason));
	registerProperty("recordVolume", FB::make_property(this, &CallAPI::getRecordVolume));
	registerProperty("referTo", FB::make_property(this, &CallAPI::getReferTo));
	registerProperty("remoteAddress", FB::make_property(this, &CallAPI::getRemoteAddress));
	registerProperty("remoteAddressAsString", FB::make_property(this, &CallAPI::getRemoteAddressAsString));
	registerProperty("remoteParams", FB::make_property(this, &CallAPI::getRemoteParams));
	registerProperty("remoteUserAgent", FB::make_property(this, &CallAPI::getRemoteUserAgent));
	registerProperty("replacedCall", FB::make_property(this, &CallAPI::getReplacedCall));
	registerProperty("state", FB::make_property(this, &CallAPI::getState));
	registerProperty("transferState", FB::make_property(this, &CallAPI::getTransferState));
	registerProperty("videoStats", FB::make_property(this, &CallAPI::getVideoStats));
	registerProperty("inConference", FB::make_property(this, &CallAPI::inConference));

	registerMethod("askedToAutoanswer", make_method(this, &CallAPI::askedToAutoanswer));
	registerMethod("hasTransferPending", make_method(this, &CallAPI::hasTransferPending));
	registerMethod("sendVfuRequest", make_method(this, &CallAPI::sendVfuRequest));
	registerMethod("setAuthenticationTokenVerified", make_method(this, &CallAPI::setAuthenticationTokenVerified));
	registerMethod("zoomVideo", make_method(this, &CallAPI::zoomVideo));
}

CallAPI::~CallAPI() {
	FBLOG_DEBUG("CallAPI::~CallAPI", "this=" << this);
	linphone_call_set_user_pointer(mCall, NULL);
	linphone_call_unref(mCall);
}

CallStatsAPIPtr CallAPI::getAudioStats() const {
	FBLOG_DEBUG("CallAPI::getAudioStats()", "this=" << this);
	return mFactory->get(linphone_call_get_audio_stats(mCall));
}

std::string CallAPI::getAuthenticationToken() const {
	FBLOG_DEBUG("CallAPI::getAuthenticationToken()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_call_get_authentication_token(mCall));
}

bool CallAPI::getAuthenticationTokenVerified() const {
	FBLOG_DEBUG("CallAPI::getAuthenticationTokenVerified()", "this=" << this);
	return linphone_call_get_authentication_token_verified(mCall) == TRUE? true : false;
}

float CallAPI::getAverageQuality() const {
	FBLOG_DEBUG("CallAPI::getCurrentQuality()", "this=" << this);
	return linphone_call_get_average_quality(mCall);
}

CallLogAPIPtr CallAPI::getCallLog() const {
	FBLOG_DEBUG("CallAPI::getCallLog()", "this=" << this);
	return mFactory->get(linphone_call_get_call_log(mCall));
}

CoreAPIPtr CallAPI::getCore() const {
	FBLOG_DEBUG("CallAPI::getCurrentQuality()", "this=" << this);
	return mFactory->get(linphone_call_get_core(mCall));
}

CallParamsAPIPtr CallAPI::getCurrentParams() const {
	FBLOG_DEBUG("CallAPI::getCurrentParams()", "this=" << this);
	return mFactory->get(linphone_call_get_current_params(mCall));
}

float CallAPI::getCurrentQuality() const {
	FBLOG_DEBUG("CallAPI::getCurrentQuality()", "this=" << this);
	return linphone_call_get_current_quality(mCall);
}

int CallAPI::getDir() const {
	FBLOG_DEBUG("CallAPI::getDir()", "this=" << this);
	return linphone_call_get_dir(mCall);
}

int CallAPI::getDuration() const {
	FBLOG_DEBUG("CallAPI::getDir()", "this=" << this);
	return linphone_call_get_duration(mCall);
}

float CallAPI::getPlayVolume() const {
	FBLOG_DEBUG("CallAPI::getPlayVolume()", "this=" << this);
	return linphone_call_get_play_volume(mCall);
}

int CallAPI::getReason() const {
	FBLOG_DEBUG("CallAPI::getReason()", "this=" << this);
	return linphone_call_get_reason(mCall);
}

float CallAPI::getRecordVolume() const {
	FBLOG_DEBUG("CallAPI::getReason()", "this=" << this);
	return linphone_call_get_record_volume(mCall);
}

std::string CallAPI::getReferTo() const {
	FBLOG_DEBUG("CallAPI::getReason()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_call_get_refer_to(mCall));
}

AddressAPIPtr CallAPI::getRemoteAddress() const {
	FBLOG_DEBUG("CallAPI::getRemoteAddress()", "this=" << this);
	return mFactory->get(linphone_call_get_remote_address(mCall));
}

std::string CallAPI::getRemoteAddressAsString() const {
	FBLOG_DEBUG("CallAPI::getRemoteAddressAsString()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_call_get_remote_address_as_string(mCall));
}

CallParamsAPIPtr CallAPI::getRemoteParams() const {
	FBLOG_DEBUG("CallAPI::getRemoteParams()", "this=" << this);
	return mFactory->get(linphone_call_get_remote_params(mCall));
}

std::string CallAPI::getRemoteUserAgent() const {
	FBLOG_DEBUG("CallAPI::getRemoteUserAgent()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_call_get_remote_user_agent(mCall));
}

CallAPIPtr CallAPI::getReplacedCall() const {
	FBLOG_DEBUG("CallAPI::getReplacedCall()", "this=" << this);
	return mFactory->get(linphone_call_get_replaced_call(mCall));
}

int CallAPI::getState() const {
	FBLOG_DEBUG("CallAPI::getState()", "this=" << this);
	return linphone_call_get_state(mCall);
}

int CallAPI::getTransferState() const {
	FBLOG_DEBUG("CallAPI::getTransferState()", "this=" << this);
	return linphone_call_get_transfer_state(mCall);
}

CallStatsAPIPtr CallAPI::getVideoStats() const {
	FBLOG_DEBUG("CallAPI::getVideoStats()", "this=" << this);
	return mFactory->get(linphone_call_get_video_stats(mCall));
}

bool CallAPI::inConference() const {
	FBLOG_DEBUG("CallAPI::inConference()", "this=" << this);
	return linphone_call_is_in_conference(mCall) == TRUE ? true : false;
}

bool CallAPI::cameraEnabled() const {
	FBLOG_DEBUG("CallAPI::cameraEnabled()", "this=" << this);
	return linphone_call_camera_enabled(mCall) == TRUE ? true : false;
}

void CallAPI::enableCamera(bool enabled) {
	FBLOG_DEBUG("CallAPI::enableCamera()", "this=" << this << "\t" << "enabled=" << enabled);
	linphone_call_enable_camera(mCall, enabled ? TRUE : FALSE);
}

bool CallAPI::echoCancellationEnabled() const {
	FBLOG_DEBUG("CallAPI::echoCancellationEnabled()", "this=" << this);
	return linphone_call_echo_cancellation_enabled(mCall) == TRUE ? true : false;
}

void CallAPI::enableEchoCancellation(bool enabled) {
	FBLOG_DEBUG("CallAPI::enableEchoCancellation()", "this=" << this << "\t" << "enabled=" << enabled);
	linphone_call_enable_echo_cancellation(mCall, enabled ? TRUE : FALSE);
}

bool CallAPI::echoLimiterEnabled() const {
	FBLOG_DEBUG("CallAPI::echoLimiterEnabled()", "this=" << this);
	return linphone_call_echo_limiter_enabled(mCall) == TRUE ? true : false;
}

void CallAPI::enableEchoLimiter(bool enabled) {
	FBLOG_DEBUG("CallAPI::enableEchoLimiter()", "this=" << this << "\t" << "enabled=" << enabled);
	linphone_call_enable_echo_limiter(mCall, enabled ? TRUE : FALSE);
}

bool CallAPI::askedToAutoanswer() const {
	FBLOG_DEBUG("CallAPI::askedToAutoanswer()", "this=" << this);
	return linphone_call_asked_to_autoanswer(mCall) == TRUE ? true : false;
}

bool CallAPI::hasTransferPending() const {
	FBLOG_DEBUG("CallAPI::hasTransferPending()", "this=" << this);
	return linphone_call_has_transfer_pending(mCall) == TRUE ? true : false;
}

void CallAPI::sendVfuRequest() {
	FBLOG_DEBUG("CallAPI::sendVfuRequest()", "this=" << this);
	linphone_call_send_vfu_request(mCall);
}

void CallAPI::setAuthenticationTokenVerified(bool verified) {
	FBLOG_DEBUG("CallAPI::setAuthenticationTokenVerified()", "this=" << this);
	linphone_call_set_authentication_token_verified(mCall, verified);
}

void CallAPI::zoomVideo(float zoom, float cx, float cy) {
	FBLOG_DEBUG("CallAPI::zoomVideo()", "this=" << this << "\t" << "zoom=" << zoom << ", cx=" << cx << ", cy=" << cy);
	linphone_call_zoom_video(mCall, zoom, &cx, &cy);
}
