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

#include <JSObject.h>
#include <SystemHelpers.h>
#include <variant_list.h>
#include <DOM/Document.h>
#include <global/config.h>
#include <sstream>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <fstream>
#include "coreapi.h"

#ifndef WIN32
#else
#include <windows.h>
void usleep(int waitTime) {
	Sleep(waitTime/1000);
}
#endif

#ifdef CORE_THREADED
#define CORE_MUTEX boost::mutex::scoped_lock scopedLock(m_core_mutex);
#else
#define CORE_MUTEX
#endif //CORE_THREADED

static boost::mutex sInstanceMutex;
static int sInstanceCount = 0;

#ifdef CORE_THREADED
void linphone_iterate_thread(CoreAPI *linphone_api) {
	FBLOG_DEBUG("linphone_thread", "start");
	FBLOG_DEBUG("linphone_thread", linphone_api);

	boost::this_thread::disable_interruption di;

	while (!boost::this_thread::interruption_requested()) {
		linphone_api->iterateWithMutex();
		usleep(20000);
		//FBLOG_DEBUG("linphone_thread", "it");
	}

	FBLOG_DEBUG("linphone_thread", "end");
}

void linphone_destroy_thread(LinphoneCore* core, boost::thread *thread, mythread_group *threads) {
	FBLOG_DEBUG("linphone_destroy_thread", "start");
	FBLOG_DEBUG("linphone_destroy_thread", core);

	if (thread != NULL) {
		thread->interrupt();
		thread->join();
		delete thread;
	}

	if(threads != NULL) {
		threads->interrupt_all();
		threads->join_all();
		delete threads;
	}

	if (core != NULL) {
		linphone_core_destroy(core);
	}

	sInstanceMutex.lock();
	--sInstanceCount;
	sInstanceMutex.unlock();
	FBLOG_DEBUG("linphone_destroy_thread", "end");
}
#endif //CORE_THREADED

///////////////////////////////////////////////////////////////////////////////
/// @fn CoreAPI::CoreAPI(const corePtr& plugin, const FB::BrowserHostPtr host)
///
/// @brief  Constructor for your JSAPI object.  You should register your methods, properties, and events
///         that should be accessible to Javascript from here.
///
/// @see FB::JSAPIAuto::registerMethod
/// @see FB::JSAPIAuto::registerProperty
/// @see FB::JSAPIAuto::registerEvent
///////////////////////////////////////////////////////////////////////////////
CoreAPI::CoreAPI(const corePtr& plugin, const FB::BrowserHostPtr& host) :
		JSAPIAuto(APIDescription(this)), m_plugin(plugin), m_host(host), mCore(NULL) {
	FBLOG_DEBUG("CoreAPI::CoreAPI()", "this=" << this);
#ifdef CORE_THREADED
	m_core_thread = NULL;
	m_threads = new mythread_group();
#else
	m_timer = FB::Timer::getTimer(20, true, boost::bind(&CoreAPI::iterate, this));
#endif
	initProxy();
}

void CoreAPI::initProxy() {
	// Read-only property
	registerProperty("version", make_property(this, &CoreAPI::getVersion));
	registerProperty("pluginVersion", make_property(this, &CoreAPI::getPluginVersion));

	// Propery
	registerProperty("magic", make_property(this, &CoreAPI::getMagic, &CoreAPI::setMagic));

	registerMethod("init", make_method(this, &CoreAPI::init));

	// Call bindings
	REGISTER_SYNC_N_ASYNC(CoreAPI, "invite", invite);
	REGISTER_SYNC_N_ASYNC(CoreAPI, "inviteAddress", inviteAddress);
	REGISTER_SYNC_N_ASYNC(CoreAPI, "inviteWithParams", inviteWithParams);
	REGISTER_SYNC_N_ASYNC(CoreAPI, "inviteAddressWithParams", inviteAddressWithParams);
	registerMethod("acceptCall", make_method(this, &CoreAPI::acceptCall));
	registerMethod("acceptCallWithParams", make_method(this, &CoreAPI::acceptCallWithParams));
	registerProperty("currentCall", make_property(this, &CoreAPI::getCurrentCall));
	registerMethod("terminateCall", make_method(this, &CoreAPI::terminateCall));
	registerMethod("terminateAllCalls", make_method(this, &CoreAPI::terminateAllCalls));
	registerMethod("redirectCall", make_method(this, &CoreAPI::redirectCall));
	registerMethod("declineCall", make_method(this, &CoreAPI::declineCall));
	registerMethod("transferCall", make_method(this, &CoreAPI::transferCall));
	registerMethod("transferCallToAnother", make_method(this, &CoreAPI::transferCallToAnother));
	registerMethod("resumeCall", make_method(this, &CoreAPI::resumeCall));
	registerMethod("pauseCall", make_method(this, &CoreAPI::pauseCall));
	registerMethod("pauseAllCalls", make_method(this, &CoreAPI::pauseAllCalls));
	registerMethod("updateCall", make_method(this, &CoreAPI::updateCall));
	registerMethod("deferCallUpdate", make_method(this, &CoreAPI::deferCallUpdate));
	registerMethod("acceptCallUpdate", make_method(this, &CoreAPI::acceptCallUpdate));
	registerMethod("createDefaultCallParameters", make_method(this, &CoreAPI::createDefaultCallParameters));
	registerProperty("incTimeout", make_property(this, &CoreAPI::getIncTimeout, &CoreAPI::setIncTimeout));
	registerProperty("inCallTimeout", make_property(this, &CoreAPI::getInCallTimeout, &CoreAPI::setInCallTimeout));
	registerProperty("maxCalls", make_property(this, &CoreAPI::getMaxCalls, &CoreAPI::setMaxCalls));

	// Conference bindings
	registerMethod("addAllToConference", make_method(this, &CoreAPI::addAllToConference));
	registerMethod("addToConference", make_method(this, &CoreAPI::addToConference));
	registerMethod("enterConference", make_method(this, &CoreAPI::enterConference));
	registerProperty("conferenceSize", make_property(this, &CoreAPI::getConferenceSize));
	registerMethod("isInConference", make_method(this, &CoreAPI::isInConference));
	registerMethod("leaveConference", make_method(this, &CoreAPI::leaveConference));
	registerMethod("removeFromConference", make_method(this, &CoreAPI::removeFromConference));
	registerMethod("terminateConference", make_method(this, &CoreAPI::terminateConference));

	// Levels bindings
	registerProperty("playLevel", make_property(this, &CoreAPI::getPlayLevel, &CoreAPI::setPlayLevel));
	registerProperty("recLevel", make_property(this, &CoreAPI::getRecLevel, &CoreAPI::setRecLevel));
	registerProperty("ringLevel", make_property(this, &CoreAPI::getRingLevel, &CoreAPI::setRingLevel));
	registerProperty("muteMic", make_property(this, &CoreAPI::isMicMuted, &CoreAPI::muteMic));
	registerProperty("micGainDb", make_property(this, &CoreAPI::getMicGainDb, &CoreAPI::setMicGainDb));
	registerProperty("playbackGainDb", make_property(this, &CoreAPI::getPlaybackGainDb, &CoreAPI::setPlaybackGainDb));

	// Video bindings
	registerMethod("videoSupported", make_method(this, &CoreAPI::videoSupported));
	registerProperty("videoPreviewEnabled", make_property(this, &CoreAPI::videoPreviewEnabled, &CoreAPI::enableVideoPreview));
	registerProperty("selfViewEnabled", make_property(this, &CoreAPI::selfViewEnabled, &CoreAPI::enableSelfView));
	registerProperty("videoEnabled", make_property(this, &CoreAPI::videoEnabled, &CoreAPI::enableVideo));
	registerProperty("nativePreviewWindowId", make_property(this, &CoreAPI::getNativePreviewWindowId, &CoreAPI::setNativePreviewWindowId));
	registerProperty("nativeVideoWindowId", make_property(this, &CoreAPI::getNativeVideoWindowId, &CoreAPI::setNativeVideoWindowId));
	registerProperty("usePreviewWindow", make_property(this, &CoreAPI::getUsePreviewWindow, &CoreAPI::setUsePreviewWindow));

	// Sound device bindings
	registerMethod("reloadSoundDevices", make_method(this, &CoreAPI::reloadSoundDevices));
	registerProperty("soundDevices", make_property(this, &CoreAPI::getSoundDevices));
	registerMethod("soundDeviceCanCapture", make_method(this, &CoreAPI::soundDeviceCanCapture));
	registerMethod("soundDeviceCanPlayback", make_method(this, &CoreAPI::soundDeviceCanPlayback));
	registerProperty("ringerDevice", make_property(this, &CoreAPI::getRingerDevice, &CoreAPI::setRingerDevice));
	registerProperty("captureDevice", make_property(this, &CoreAPI::getCaptureDevice, &CoreAPI::setCaptureDevice));
	registerProperty("playbackDevice", make_property(this, &CoreAPI::getPlaybackDevice, &CoreAPI::setPlaybackDevice));

	// Video device bindings
	registerMethod("reloadVideoDevices", make_method(this, &CoreAPI::reloadVideoDevices));
	registerProperty("videoDevices", make_property(this, &CoreAPI::getVideoDevices));
	registerProperty("videoDevice", make_property(this, &CoreAPI::getVideoDevice, &CoreAPI::setVideoDevice));

	// Codecs bindings
	registerProperty("audioCodecs", make_property(this, &CoreAPI::getAudioCodecs, &CoreAPI::setAudioCodecs));
	registerProperty("videoCodecs", make_property(this, &CoreAPI::getVideoCodecs, &CoreAPI::setVideoCodecs));
	registerMethod("payloadTypeEnabled", make_method(this, &CoreAPI::payloadTypeEnabled));
	registerMethod("enablePayloadType", make_method(this, &CoreAPI::enablePayloadType));

	// ProxyConfig bindings
	registerMethod("addProxyConfig", make_method(this, &CoreAPI::addProxyConfig));
	registerMethod("clearProxyConfig", make_method(this, &CoreAPI::clearProxyConfig));
	registerMethod("removeProxyConfig", make_method(this, &CoreAPI::removeProxyConfig));
	registerProperty("proxyConfigList", make_property(this, &CoreAPI::getProxyConfigList));
	registerProperty("defaultProxy", make_property(this, &CoreAPI::getDefaultProxy, &CoreAPI::setDefaultProxy));
	registerMethod("refreshRegisters", make_method(this, &CoreAPI::refreshRegisters));

	// Network bindings
	registerProperty("audioPort", make_property(this, &CoreAPI::getAudioPort, &CoreAPI::setAudioPort));
	registerProperty("videoPort", make_property(this, &CoreAPI::getVideoPort, &CoreAPI::setVideoPort));
	registerProperty("downloadBandwidth", make_property(this, &CoreAPI::getDownloadBandwidth, &CoreAPI::setDownloadBandwidth));
	registerProperty("uploadBandwidth", make_property(this, &CoreAPI::getUploadBandwidth, &CoreAPI::setUploadBandwidth));
	registerProperty("downloadPtime", make_property(this, &CoreAPI::getDownloadPtime, &CoreAPI::setDownloadPtime));
	registerProperty("uploadPtime", make_property(this, &CoreAPI::getUploadPtime, &CoreAPI::setUploadPtime));
	registerProperty("mtu", make_property(this, &CoreAPI::getMtu, &CoreAPI::setMtu));
	registerProperty("stunServer", make_property(this, &CoreAPI::getStunServer, &CoreAPI::setStunServer));
	registerProperty("relayAddr", make_property(this, &CoreAPI::getRelayAddr, &CoreAPI::setRelayAddr));
	registerProperty("natAddress", make_property(this, &CoreAPI::getNatAddress, &CoreAPI::setNatAddress));
	registerProperty("guessHostname", make_property(this, &CoreAPI::getGuessHostname, &CoreAPI::setGuessHostname));
	registerProperty("ipv6Enabled", make_property(this, &CoreAPI::ipv6Enabled, &CoreAPI::enableIpv6));
	registerProperty("keepAliveEnabled", make_property(this, &CoreAPI::keepAliveEnabled, &CoreAPI::enableKeepAlive));
	registerProperty("audioDscp", make_property(this, &CoreAPI::getAudioDscp, &CoreAPI::setAudioDscp));
	registerProperty("sipDscp", make_property(this, &CoreAPI::getSipDscp, &CoreAPI::setSipDscp));
	registerProperty("videoDscp", make_property(this, &CoreAPI::getVideoDscp, &CoreAPI::setVideoDscp));
	registerProperty("sipPort", make_property(this, &CoreAPI::getSipPort, &CoreAPI::setSipPort));
	registerProperty("adaptiveRateControl", make_property(this, &CoreAPI::adaptiveRateControlEnabled, &CoreAPI::enableAdaptiveRateControl));
	registerProperty("networkReachable", make_property(this, &CoreAPI::isNetworkReachable, &CoreAPI::setNetworkReachable));
	registerProperty("audioAdaptiveJittcomp", make_property(this, &CoreAPI::audioAdaptiveJittcompEnabled, &CoreAPI::enableAudioAdaptiveJittcomp));
	registerProperty("videoAdaptiveJittcomp", make_property(this, &CoreAPI::videoAdaptiveJittcompEnabled, &CoreAPI::enableVideoAdaptiveJittcomp));
	registerProperty("audioJittcomp", make_property(this, &CoreAPI::getAudioJittcomp, &CoreAPI::setAudioJittcomp));
	registerProperty("videoJittcomp", make_property(this, &CoreAPI::getVideoJittcomp, &CoreAPI::setVideoJittcomp));

	// AuthInfo bindings
	registerMethod("addAuthInfo", make_method(this, &CoreAPI::addAuthInfo));
	registerMethod("abortAuthentication", make_method(this, &CoreAPI::abortAuthentication));
	registerMethod("removeAuthInfo", make_method(this, &CoreAPI::removeAuthInfo));
	registerMethod("findAuthInfo", make_method(this, &CoreAPI::findAuthInfo));
	registerProperty("authInfoList", make_property(this, &CoreAPI::getAuthInfoList));
	registerMethod("clearAllAuthInfo", make_method(this, &CoreAPI::clearAllAuthInfo));

	// File bindings
	REGISTER_PROPERTY_FILE(CoreAPI, "ring", getRing, setRing);
	REGISTER_PROPERTY_FILE(CoreAPI, "ringback", getRingback, setRingback);
	REGISTER_PROPERTY_FILE(CoreAPI, "rootCa", getRootCa, setRootCa);
	REGISTER_PROPERTY_FILE(CoreAPI, "staticPicture", getStaticPicture, setStaticPicture);
	REGISTER_PROPERTY_FILE(CoreAPI, "zrtpSecretsFile", getZrtpSecretsFile, setZrtpSecretsFile);

	// Initiator bindings
	registerMethod("newProxyConfig", make_method(this, &CoreAPI::newProxyConfig));
	registerMethod("newAuthInfo", make_method(this, &CoreAPI::newAuthInfo));

	// Dtmf
	registerMethod("sendDtmf", make_method(this, &CoreAPI::sendDtmf));
	registerMethod("playDtmf", make_method(this, &CoreAPI::playDtmf));
	registerMethod("stopDtmf", make_method(this, &CoreAPI::stopDtmf));
	registerProperty("useInfoForDtmf", make_property(this, &CoreAPI::getUseInfoForDtmf, &CoreAPI::setUseInfoForDtmf));
	registerProperty("useRfc2833ForDtmf", make_property(this, &CoreAPI::getUseRfc2833ForDtmf, &CoreAPI::setUseRfc2833ForDtmf));

	// Miscs
	registerProperty("echoCancellationEnabled", make_property(this, &CoreAPI::echoCancellationEnabled, &CoreAPI::enableEchoCancellation));
	registerProperty("echoLimiterEnabled", make_property(this, &CoreAPI::echoLimiterEnabled, &CoreAPI::enableEchoLimiter));
	registerProperty("staticPictureFps", make_property(this, &CoreAPI::getStaticPictureFps, &CoreAPI::setStaticPictureFps));

	// Download
	registerMethod("download", make_method(this, &CoreAPI::download));
}

int CoreAPI::init() {
	FBLOG_DEBUG("CoreAPI::init()", "this=" << this);
	boost::mutex::scoped_lock scoped_instance_count_lock(sInstanceMutex);

	if (sInstanceCount == 0) {
		++sInstanceCount;

		// Initialize callback table
		memset(&mVtable, 0, sizeof(LinphoneCoreVTable));
		mVtable.global_state_changed = CoreAPI::wrapper_global_state_changed;
		mVtable.registration_state_changed = CoreAPI::wrapper_registration_state_changed;
		mVtable.call_state_changed = CoreAPI::wrapper_call_state_changed;
		mVtable.notify_presence_recv = CoreAPI::wrapper_notify_presence_recv;
		mVtable.new_subscription_request = CoreAPI::wrapper_new_subscription_request;
		mVtable.auth_info_requested = CoreAPI::wrapper_auth_info_requested;
		mVtable.call_log_updated = CoreAPI::wrapper_call_log_updated;
		mVtable.text_received = CoreAPI::wrapper_text_received;
		mVtable.dtmf_received = CoreAPI::wrapper_dtmf_received;
		mVtable.refer_received = CoreAPI::wrapper_refer_received;
		mVtable.buddy_info_updated = CoreAPI::wrapper_buddy_info_updated;
		mVtable.notify_recv = CoreAPI::wrapper_notify_recv;
		mVtable.display_status = CoreAPI::wrapper_display_status;
		mVtable.display_message = CoreAPI::wrapper_display_message;
		mVtable.display_warning = CoreAPI::wrapper_display_warning;
		mVtable.display_url = CoreAPI::wrapper_display_url;
		mVtable.show = CoreAPI::wrapper_show;
		mVtable.call_encryption_changed = CoreAPI::wrapper_call_encryption_changed;

		mCore = linphone_core_new(&mVtable, NULL, NULL, (void *) this);
		if (linphone_core_get_user_data(mCore) != this) {
			FBLOG_ERROR("CoreAPI::init()", "Too old version of linphone core!");
			--sInstanceCount;
			return 1;
		}

		int port = 5000 + rand() % 5000;
		FBLOG_DEBUG("CoreAPI::init()", "port=" << port);
		linphone_core_set_sip_port(mCore, port);

#ifdef CORE_THREADED
		m_core_thread = new boost::thread(linphone_iterate_thread, this);
#else
		m_timer->start();
#endif
		return 0;
	} else {
		FBLOG_ERROR("CoreAPI::init()", "Already started linphone instance");
		return 2;
	}
}

///////////////////////////////////////////////////////////////////////////////
/// @fn CoreAPI::~CoreAPI()
///
/// @brief  Destructor.  Remember that this object will not be released until
///         the browser is done with it; this will almost definitely be after
///         the plugin is released.
///////////////////////////////////////////////////////////////////////////////
CoreAPI::~CoreAPI() {
	FBLOG_DEBUG("CoreAPI::~CoreAPI()", "this=" << this);
	if (mCore != NULL) {
		linphone_core_set_user_data(mCore, NULL);

#ifdef CORE_THREADED
		boost::thread t(linphone_destroy_thread, mCore, m_core_thread, m_threads);
#else
		m_timer->stop();
		linphone_core_destroy(mCore);

		sInstanceMutex.lock();
		--sInstanceCount;
		sInstanceMutex.unlock();
#endif
	}
}

///////////////////////////////////////////////////////////////////////////////
/// @fn corePtr CoreAPI::getPlugin()
///
/// @brief  Gets a reference to the plugin that was passed in when the object
///         was created.  If the plugin has already been released then this
///         will throw a FB::script_error that will be translated into a
///         javascript exception in the page.
///////////////////////////////////////////////////////////////////////////////
corePtr CoreAPI::getPlugin() {
	corePtr plugin(m_plugin.lock());
	if (!plugin) {
		throw FB::script_error("The plugin is invalid");
	}
	return plugin;
}

const std::string &CoreAPI::getMagic() const {
	FBLOG_DEBUG("CoreAPI::getMagic()", "this=" << this);
	return m_magic;
}

void CoreAPI::setMagic(const std::string &magic) {
	FBLOG_DEBUG("CoreAPI::setMagic()", "this=" << this << "\t" << "magic=" << magic);
	m_magic = magic;
}

CoreAPIPtr CoreAPI::get(LinphoneCore *core) {
	if (core == NULL)
		return CoreAPIPtr();

	void *ptr = linphone_core_get_user_data(core);
	CoreAPIPtr shared_ptr;
	if (ptr != NULL) {
		shared_ptr = boost::static_pointer_cast<CoreAPI>(reinterpret_cast<CoreAPI *>(ptr)->shared_from_this());
	}
	return shared_ptr;
}

/*
 *
 * Call functions
 *
 */

CallAPIPtr CoreAPI::invite(const std::string &url) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::invite", "this=" << this << "\t" << "url=" << url);
	LinphoneCall *call = linphone_core_invite(mCore, url.c_str());
	CallAPIPtr shared_call = CallAPI::get(call);
	return shared_call;
}

CallAPIPtr CoreAPI::inviteAddress(const AddressAPIPtr &address) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::inviteAddress", "this=" << this << "\t" << "address=" << address);
	LinphoneCall *call = linphone_core_invite_address(mCore, address->getRef());
	CallAPIPtr shared_call = CallAPI::get(call);
	return shared_call;
}

CallAPIPtr CoreAPI::inviteWithParams(const std::string &url, const CallParamsAPIPtr &params) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::invite", "this=" << this << "\t" << "url=" << url << "\t" << "params=" << params);
	LinphoneCall *call = linphone_core_invite_with_params(mCore, url.c_str(), params->getRef());
	CallAPIPtr shared_call = CallAPI::get(call);
	return shared_call;
}

CallAPIPtr CoreAPI::inviteAddressWithParams(const AddressAPIPtr &address, const CallParamsAPIPtr &params) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::inviteAddress", "this=" << this << "\t" << "address=" << address << "\t" << "params=" << params);
	LinphoneCall *call = linphone_core_invite_address_with_params(mCore, address->getRef(), params->getRef());
	CallAPIPtr shared_call = CallAPI::get(call);
	return shared_call;
}

int CoreAPI::acceptCall(const CallAPIPtr &call) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::acceptCall", "this=" << this << "\t" << "call=" << call);
	return linphone_core_accept_call(mCore, call->getRef());
}

int CoreAPI::acceptCallWithParams(const CallAPIPtr &call, const CallParamsAPIPtr &params) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::acceptCallWithParams", "this=" << this << "\t" << "call=" << call << "\t" << "params=" << params);
	return linphone_core_accept_call_with_params(mCore, call->getRef(), params->getRef());
}

CallAPIPtr CoreAPI::getCurrentCall() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getCurrentCall", "this=" << this);
	return CallAPI::get(linphone_core_get_current_call(mCore));
}

int CoreAPI::terminateCall(const CallAPIPtr &call) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::terminateCall", "this=" << this << "\t" << "call=" << call);
	return linphone_core_terminate_call(mCore, call->getRef());
}

int CoreAPI::terminateAllCalls() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::terminateAllCalls", "this=" << this);
	return linphone_core_terminate_all_calls(mCore); 
}

int CoreAPI::redirectCall(const CallAPIPtr &call, const std::string &uri) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::redirectCall", "this=" << this << "\t" << "call=" << call << "\t" << "uri=" << uri);
	return linphone_core_redirect_call(mCore, call->getRef(), uri.c_str());
}

int CoreAPI::declineCall(const CallAPIPtr &call, int reason) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::declineCall", "this=" << this << "\t" << "call=" << call << "\t" << "reason=" << reason);
	return linphone_core_decline_call(mCore, call->getRef(), (LinphoneReason)reason);
}

int CoreAPI::transferCall(const CallAPIPtr &call, const std::string &uri) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::transferCall", "this=" << this << "\t" << "call=" << call << "\t" << "uri=" << uri);
	return linphone_core_transfer_call(mCore, call->getRef(), uri.c_str());
}

int CoreAPI::transferCallToAnother(const CallAPIPtr &call, const CallAPIPtr &dest) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::transferCallToAnother", "this=" << this << "\t" << "call=" << call << "\t" << "dest=" << dest);
	return linphone_core_transfer_call_to_another(mCore, call->getRef(), dest->getRef());
}

int CoreAPI::resumeCall(const CallAPIPtr &call) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::resumeCall", "this=" << this << "\t" << "call=" << call);
	return linphone_core_resume_call(mCore, call->getRef());
}

int CoreAPI::pauseCall(const CallAPIPtr &call) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::pauseCall", "this=" << this << "\t" << "call=" << call);
	return linphone_core_pause_call(mCore, call->getRef());
}

int CoreAPI::pauseAllCalls() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::pauseAllCalls", "this=" << this);
	return linphone_core_pause_all_calls(mCore);
}

int CoreAPI::updateCall(const CallAPIPtr &call, const CallParamsAPIPtr &params) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::updateCall", "this=" << this << "\t" << "call=" << call << "\t" << "params=" << params);
	return linphone_core_update_call(mCore, call->getRef(), params->getRef());
}

int CoreAPI::deferCallUpdate(const CallAPIPtr &call) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::deferCallUpdate", "this=" << this << "\t" << "call=" << call);
	return linphone_core_defer_call_update(mCore, call->getRef());
}

int CoreAPI::acceptCallUpdate(const CallAPIPtr &call, const CallParamsAPIPtr &params) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::acceptCallUpdate", "this=" << this << "\t" << "call=" << call << "\t" << "params=" << params);
	return linphone_core_accept_call_update(mCore, call->getRef(), params->getRef());
}

CallParamsAPIPtr CoreAPI::createDefaultCallParameters() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::createDefaultCallParameters", "this=" << this);
	return CallParamsAPI::get(linphone_core_create_default_call_parameters(mCore));
}

void CoreAPI::setIncTimeout(int timeout) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setIncTimeout", "this=" << this << "\t" << "timeout=" << timeout);
	linphone_core_set_inc_timeout(mCore, timeout);
}

int CoreAPI::getIncTimeout() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getIncTimeout", "this=" << this);
	return linphone_core_get_inc_timeout(mCore);
}

void CoreAPI::setInCallTimeout(int timeout) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setInCallTimeout", "this=" << this << "\t" << "timeout=" << timeout);
	linphone_core_set_in_call_timeout(mCore, timeout);
}

int CoreAPI::getInCallTimeout() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getInCallTimeout", "this=" << this);
	return linphone_core_get_in_call_timeout(mCore);
}

int CoreAPI::getMaxCalls() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getMaxCalls", "this=" << this);
	return linphone_core_get_max_calls(mCore);
}

void CoreAPI::setMaxCalls(int max) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setMaxCalls", "this=" << this << "\t" << "max=" << max);
	linphone_core_set_max_calls(mCore, max);
}

/*
 *
 * Conference functions
 *
 */

int CoreAPI::addAllToConference() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::addAllToConference", "this=" << this);
	return linphone_core_add_all_to_conference(mCore);
}

int CoreAPI::addToConference(const CallAPIPtr &call) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::addToConference", "this=" << this << "\t" << "call=" << call);
	return linphone_core_add_to_conference(mCore, call->getRef());
}

int CoreAPI::enterConference() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enterConference", "this=" << this);
	return linphone_core_enter_conference(mCore);
}

int CoreAPI::getConferenceSize() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getConferenceSize", "this=" << this);
	return linphone_core_get_conference_size(mCore);
}

bool CoreAPI::isInConference() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::isInConference", "this=" << this);
	return linphone_core_is_in_conference(mCore) == TRUE ? true : false;
}

int CoreAPI::leaveConference() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::leaveConference", "this=" << this);
	return linphone_core_leave_conference(mCore);
}

int CoreAPI::removeFromConference(const CallAPIPtr &call) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::removeFromConference", "this=" << this << "\t" << "call=" << call);
	return linphone_core_remove_from_conference(mCore, call->getRef());
}

int CoreAPI::terminateConference() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::terminateConference", "this=" << this);
	return linphone_core_terminate_conference(mCore);
}

/*
 *
 * Level functions
 *
 */

void CoreAPI::setPlayLevel(int level) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setPlayLevel", "this=" << this << "\t" << "level=" << level);
	linphone_core_set_play_level(mCore, level);
}

int CoreAPI::getPlayLevel() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getPlayLevel", "this=" << this);
	return linphone_core_get_play_level(mCore);
}

void CoreAPI::setRecLevel(int level) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setRecLevel", "this=" << this << "\t" << "level=" << level);
	linphone_core_set_rec_level(mCore, level);
}

int CoreAPI::getRecLevel() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getRecLevel", "this=" << this);
	return linphone_core_get_rec_level(mCore);
}

void CoreAPI::setRingLevel(int level) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setRingLevel", "this=" << this << "\t" << "level=" << level);
	linphone_core_set_ring_level(mCore, level);
}

int CoreAPI::getRingLevel() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getRingLevel", "this=" << this);
	return linphone_core_get_ring_level(mCore);
}

void CoreAPI::muteMic(bool muted) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::muteMic", "this=" << this << "\t" << "muted=" << muted);
	linphone_core_mute_mic(mCore, muted ? TRUE : FALSE);
}

bool CoreAPI::isMicMuted() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::isMicMuted", "this=" << this);
	return linphone_core_is_mic_muted(mCore) == TRUE ? true : false;
}

float CoreAPI::getMicGainDb() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getMicGainDb", "this=" << this);
	return linphone_core_get_mic_gain_db(mCore);
}

void CoreAPI::setMicGainDb(float gain) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setMicGainDb", "this=" << this << "\t" << "gain=" << gain);
	return linphone_core_set_mic_gain_db(mCore, gain);
}

float CoreAPI::getPlaybackGainDb() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getPlaybackGainDb", "this=" << this);
	return linphone_core_get_playback_gain_db(mCore);
}

void CoreAPI::setPlaybackGainDb(float gain) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setPlaybackGainDb", "this=" << this << "\t" << "gain=" << gain);
	return linphone_core_set_playback_gain_db(mCore, gain);
}

/*
 *
 * Video functions
 *
 */

bool CoreAPI::videoSupported() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::videoSupported()", "this=" << this);
	return linphone_core_video_supported(mCore) == TRUE ? true : false;
}
void CoreAPI::enableVideo(bool enable) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableVideo()", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_enable_video(mCore, enable ? TRUE : FALSE, enable ? TRUE : FALSE);
}

bool CoreAPI::videoEnabled() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::videoEnabled()", "this=" << this);
	return linphone_core_video_enabled(mCore) == TRUE ? true : false;
}

void CoreAPI::enableVideoPreview(bool enable) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableVideoPreview()", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_enable_video_preview(mCore, enable ? TRUE : FALSE);
}

bool CoreAPI::videoPreviewEnabled() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::videoPreviewEnabled()", "this=" << this);
	return linphone_core_video_preview_enabled(mCore) == TRUE ? true : false;
}

void CoreAPI::enableSelfView(bool enable) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableSelfView()", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_enable_self_view(mCore, enable ? TRUE : FALSE);
}

bool CoreAPI::selfViewEnabled() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::selfViewEnabled()", "this=" << this);
	return linphone_core_self_view_enabled(mCore) == TRUE ? true : false;
}

void CoreAPI::setNativeVideoWindowId(unsigned long id) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setNativeVideoWindowId()", "this=" << this << "\t" << "id=" << id);
	linphone_core_set_native_video_window_id(mCore, id);
}

unsigned long CoreAPI::getNativeVideoWindowId() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getNativeVideoWindowId()", "this=" << this);
	return linphone_core_get_native_video_window_id(mCore);
}

void CoreAPI::setNativePreviewWindowId(unsigned long id) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setNativePreviewWindowId()", "this=" << this << "\t" << "id=" << id);
	linphone_core_set_native_preview_window_id(mCore, id);
}

unsigned long CoreAPI::getNativePreviewWindowId() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getNativePreviewWindowId()", "this=" << this);
	return linphone_core_get_native_preview_window_id(mCore);
}

bool CoreAPI::getUsePreviewWindow() const {
	CORE_MUTEX
    
	FBLOG_DEBUG("CoreAPI::getUsePreviewWindow()", "this=" << this);
	return FALSE; // Don't have API yet
}

void CoreAPI::setUsePreviewWindow(bool enable) {
	CORE_MUTEX
    
	FBLOG_DEBUG("CoreAPI::setUsePreviewWindow()", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_use_preview_window(mCore, enable);
}

/*
 *
 * Sound device functions
 *
 */

void CoreAPI::reloadSoundDevices() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::reloadSoundDevices()", "this=" << this);

	linphone_core_reload_sound_devices(mCore);
}

FB::VariantList CoreAPI::getSoundDevices() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getSoundDevices()", "this=" << this);
	FB::VariantList list;
	const char **devlist = linphone_core_get_sound_devices(mCore);
	while (devlist != NULL && *devlist != NULL) {
		list.push_back(std::string(*devlist++));
	}

	return list;
}

bool CoreAPI::soundDeviceCanCapture(const std::string &devid) const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::soundDeviceCanCapture()", "this=" << this << "\t" << "devid=" << devid);
	return linphone_core_sound_device_can_capture(mCore, devid.c_str()) == TRUE ? true : false;
}

bool CoreAPI::soundDeviceCanPlayback(const std::string &devid) const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::soundDeviceCanPlayback()", "this=" << this << "\t" << "devid=" << devid);
	return linphone_core_sound_device_can_playback(mCore, devid.c_str()) == TRUE ? true : false;
}

void CoreAPI::setRingerDevice(const std::string &devid) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setRingerDevice()", "this=" << this << "\t" << "devid=" << devid);
	linphone_core_set_ringer_device(mCore, devid.c_str());
}

void CoreAPI::setPlaybackDevice(const std::string &devid) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setPlaybackDevice()", "this=" << this << "\t" << "devid=" << devid);
	linphone_core_set_playback_device(mCore, devid.c_str());
}

void CoreAPI::setCaptureDevice(const std::string &devid) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setCaptureDevice()", "this=" << this << "\t" << "devid=" << devid);
	linphone_core_set_capture_device(mCore, devid.c_str());
}

std::string CoreAPI::getRingerDevice() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getRingerDevice()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_ringer_device(mCore));
}

std::string CoreAPI::getPlaybackDevice() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getPlaybackDevice()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_playback_device(mCore));
}

std::string CoreAPI::getCaptureDevice() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getPlaybackDevice()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_capture_device(mCore));
}

/*
 *
 * Video device functions
 *
 */

void CoreAPI::reloadVideoDevices() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::reloadVideoDevices()", "this=" << this);

	linphone_core_reload_video_devices(mCore);
}

FB::VariantList CoreAPI::getVideoDevices() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getVideoDevices()", "this=" << this);

	FB::VariantList list;
	const char **devlist = linphone_core_get_video_devices(mCore);
	while (devlist != NULL && *devlist != NULL) {
		list.push_back(std::string(*devlist++));
	}

	return list;
}

void CoreAPI::setVideoDevice(const std::string &devid) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setVideoDevice()", "this=" << this << "\t" << "devid=" << devid);
	linphone_core_set_video_device(mCore, devid.c_str());
}

std::string CoreAPI::getVideoDevice() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getVideoDevice()", "this=" << this);

	return CHARPTR_TO_STRING(linphone_core_get_video_device(mCore));
}

/*
 *
 * Codecs functions
 *
 */

FB::VariantList CoreAPI::getAudioCodecs() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getAudioCodecs()", "this=" << this);
	FB::VariantList list;
	for (const MSList *node = linphone_core_get_audio_codecs(mCore); node != NULL; node = ms_list_next(node)) {
		list.push_back(PayloadTypeAPI::get(reinterpret_cast<PayloadType*>(node->data)));
	}
	return list;
}

FB::VariantList CoreAPI::getVideoCodecs() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getVideoCodecs()", "this=" << this);
	FB::VariantList list;
	for (const MSList *node = linphone_core_get_video_codecs(mCore); node != NULL; node = ms_list_next(node)) {
		list.push_back(PayloadTypeAPI::get(reinterpret_cast<PayloadType*>(node->data)));
	}
	return list;
}

void CoreAPI::setAudioCodecs(const std::vector<FB::JSAPIPtr> &list) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::set_audio_codecs()", "this=" << this << "\t" << "list.size()=" << list.size());
	MSList *mslist = NULL;
	for (auto it = list.begin(); it != list.end(); ++it) {
		PayloadTypeAPIPtr payloadType = boost::dynamic_pointer_cast<PayloadTypeAPI>(*it);
		if (payloadType != NULL) {
			mslist = ms_list_append(mslist, payloadType->getRef());
		} else {
			FBLOG_DEBUG("CoreAPI::set_audio_codecs()", "Not PayloadTypeAPI");
		}
	}

	linphone_core_set_audio_codecs(mCore, mslist);
}

void CoreAPI::setVideoCodecs(const std::vector<FB::JSAPIPtr> &list) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setVideoCodecs()", "this=" << this << "\t" << "list.size()=" << list.size());
	MSList *mslist = NULL;
	for (auto it = list.begin(); it != list.end(); ++it) {
		PayloadTypeAPIPtr payloadType = boost::dynamic_pointer_cast<PayloadTypeAPI>(*it);
		if (payloadType != NULL) {
			mslist = ms_list_append(mslist, payloadType->getRef());
		} else {
			FBLOG_DEBUG("CoreAPI::setVideoCodecs()", "Not PayloadTypeAPI");
		}
	}

	linphone_core_set_video_codecs(mCore, mslist);
}

bool CoreAPI::payloadTypeEnabled(const PayloadTypeAPIPtr &payloadType) const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::payloadTypeEnabled()", "this=" << this << "\t" << "payloadType=" << payloadType);
	return linphone_core_payload_type_enabled(mCore, payloadType->getRef()) == TRUE ? true : false;
}

void CoreAPI::enablePayloadType(const PayloadTypeAPIPtr &payloadType, bool enable) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enablePayloadType()", "this=" << this << "\t" << "payloadType=" << payloadType << "\t" << "enable=" << enable);
	linphone_core_enable_payload_type(mCore, payloadType->getRef(), enable ? TRUE : FALSE);
}

/*
 *
 * Proxy functions
 *
 */

int CoreAPI::addProxyConfig(const ProxyConfigAPIPtr &config) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::addProxyConfig()", "this=" << this << "\t" << "config=" << config);
	return linphone_core_add_proxy_config(mCore, config->getRef());
}

void CoreAPI::clearProxyConfig() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::clearProxyConfig()", "this=" << this);
	linphone_core_clear_proxy_config(mCore);
}

void CoreAPI::removeProxyConfig(const ProxyConfigAPIPtr &config) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::removeProxyConfig()", "this=" << this << "\t" << "config=" << config);
	linphone_core_remove_proxy_config(mCore, config->getRef());
}

FB::VariantList CoreAPI::getProxyConfigList() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getProxyConfigList()", "this=" << this);
	FB::VariantList list;
	for (const MSList *node = linphone_core_get_proxy_config_list(mCore); node != NULL; node = ms_list_next(node)) {
		list.push_back(ProxyConfigAPI::get(reinterpret_cast<LinphoneProxyConfig*>(node->data)));
	}
	return list;
}

void CoreAPI::setDefaultProxy(const ProxyConfigAPIPtr &config) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setDefaultProxy()", "this=" << this << "\t" << "config=" << config);
	linphone_core_set_default_proxy(mCore, config->getRef());
}

ProxyConfigAPIPtr CoreAPI::getDefaultProxy() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getDefaultProxy()", "this=" << this);
	LinphoneProxyConfig *ptr = NULL;
	linphone_core_get_default_proxy(mCore, &ptr);
	if (ptr != NULL)
		return ProxyConfigAPI::get(ptr);
	return ProxyConfigAPIPtr();
}

void CoreAPI::setPrimaryContact(const std::string &contact) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setPrimaryContact()", "this=" << this << "\t" << "contact=" << contact);
	linphone_core_set_primary_contact(mCore, contact.c_str());
}

std::string CoreAPI::getPrimaryContact() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getPrimaryContact()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_primary_contact(mCore));
}

void CoreAPI::refreshRegisters() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::refreshRegisters()", "this=" << this);
	linphone_core_refresh_registers(mCore);
}

/*
 *
 * Network functions
 *
 */

void CoreAPI::setAudioPort(int port) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setAudioPort()", "this=" << this << "\t" << "port=" << port);
	linphone_core_set_audio_port(mCore, port);
}

int CoreAPI::getAudioPort() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getAudioPort()", "this=" << this);
	return linphone_core_get_audio_port(mCore);
}

void CoreAPI::setVideoPort(int port) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setVideoPort()", "this=" << this << "\t" << "port=" << port);
	linphone_core_set_video_port(mCore, port);
}

int CoreAPI::getVideoPort() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getVideoPort()", "this=" << this);
	return linphone_core_get_video_port(mCore);
}

void CoreAPI::setDownloadBandwidth(int bandwidth) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setDownloadBandwidth()", "this=" << this << "\t" << "bandwidth=" << bandwidth);
	linphone_core_set_download_bandwidth(mCore, bandwidth);
}

int CoreAPI::getDownloadBandwidth() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getDownloadBandwidth()", "this=" << this);
	return linphone_core_get_download_bandwidth(mCore);
}

void CoreAPI::setUploadBandwidth(int bandwidth) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setUploadBandwidth()", "this=" << this << "\t" << "bandwidth=" << bandwidth);
	linphone_core_set_upload_bandwidth(mCore, bandwidth);
}

int CoreAPI::getUploadBandwidth() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getUploadBandwidth()", "this=" << this);
	return linphone_core_get_upload_bandwidth(mCore);
}

void CoreAPI::setDownloadPtime(int ptime) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setDownloadPtime()", "this=" << this << "\t" << "ptime=" << ptime);
	linphone_core_set_download_ptime(mCore, ptime);
}

int CoreAPI::getDownloadPtime() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getDownloadPtime()", "this=" << this);
	return linphone_core_get_download_ptime(mCore);
}

void CoreAPI::setUploadPtime(int ptime) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setUploadPtime()", "this=" << this << "\t" << "ptime=" << ptime);
	linphone_core_set_upload_ptime(mCore, ptime);
}

int CoreAPI::getUploadPtime() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getUploadPtime()", "this=" << this);
	return linphone_core_get_upload_ptime(mCore);
}

void CoreAPI::setMtu(int mtu) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setMtu()", "this=" << this << "\t" << "mtu=" << mtu);
	return linphone_core_set_mtu(mCore, mtu);
}

int CoreAPI::getMtu() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getMtu()", "this=" << this);
	return linphone_core_get_mtu(mCore);
}

void CoreAPI::setStunServer(const std::string &server) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setStunServer()", "this=" << this << "\t" << "server=" << server);
	return linphone_core_set_stun_server(mCore, server.c_str());
}

std::string CoreAPI::getStunServer() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getStunServer()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_stun_server(mCore));
}

void CoreAPI::setRelayAddr(const std::string &addr) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setRelayAddr()", "this=" << this << "\t" << "addr=" << addr);
	linphone_core_set_relay_addr(mCore, addr.c_str());
}

std::string CoreAPI::getRelayAddr() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getRelayAddr()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_relay_addr(mCore));
}

void CoreAPI::setNatAddress(const std::string &address) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setNatAddress()", "this=" << this << "\t" << "address=" << address);
	return linphone_core_set_nat_address(mCore, address.c_str());
}

std::string CoreAPI::getNatAddress() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getNatAddress()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_nat_address(mCore));
}

void CoreAPI::setGuessHostname(bool guess) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setGuessHostname()", "this=" << this << "\t" << "guess=" << guess);
	return linphone_core_set_guess_hostname(mCore, guess ? TRUE : FALSE);
}

bool CoreAPI::getGuessHostname() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getGuessHostname()", "this=" << this);
	return linphone_core_get_guess_hostname(mCore) == TRUE ? true : false;
}

void CoreAPI::enableIpv6(bool enable) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableIpv6()", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_enable_ipv6(mCore, enable ? TRUE : FALSE);
}

bool CoreAPI::ipv6Enabled() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::ipv6Enabled()", "this=" << this);
	return linphone_core_ipv6_enabled(mCore) == TRUE ? true : false;
}

void CoreAPI::enableKeepAlive(bool enable) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableKeepAlive()", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_enable_keep_alive(mCore, enable ? TRUE : FALSE);
}

bool CoreAPI::keepAliveEnabled() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::keepAliveEnabled()", "this=" << this);
	return linphone_core_keep_alive_enabled(mCore) == TRUE ? true : false;
}

void CoreAPI::setAudioDscp(int dscp) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setAudioDscp()", "this=" << this << "\t" << "dscp=" << dscp);
	linphone_core_set_audio_dscp(mCore, dscp);
}

int CoreAPI::getAudioDscp() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getAudioDscp()", "this=" << this);
	return linphone_core_get_audio_dscp(mCore);
}

void CoreAPI::setSipDscp(int dscp) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setSipDscp()", "this=" << this << "\t" << "dscp=" << dscp);
	linphone_core_set_sip_dscp(mCore, dscp);
}

int CoreAPI::getSipDscp() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getSipDscp()", "this=" << this);
	return linphone_core_get_sip_dscp(mCore);
}

void CoreAPI::setVideoDscp(int dscp) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setVideoDscp()", "this=" << this << "\t" << "dscp=" << dscp);
	linphone_core_set_video_dscp(mCore, dscp);
}

int CoreAPI::getVideoDscp() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getVideoDscp()", "this=" << this);
	return linphone_core_get_video_dscp(mCore);
}

void CoreAPI::setSipPort(int port) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setSipPort()", "this=" << this << "\t" << "port=" << port);
	linphone_core_set_sip_port(mCore, port);
}

int CoreAPI::getSipPort() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getSipPort()", "this=" << this);
	return linphone_core_get_sip_port(mCore);
}

bool CoreAPI::adaptiveRateControlEnabled() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::adaptiveRateControlEnabled()", "this=" << this);
	return linphone_core_adaptive_rate_control_enabled(mCore) == TRUE ? true : false;
}

void CoreAPI::enableAdaptiveRateControl(bool enable) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableAdaptiveRateControl()", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_enable_adaptive_rate_control(mCore, enable ? TRUE : FALSE);
}

bool CoreAPI::isNetworkReachable() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::isNetworkReachable()", "this=" << this);
	return linphone_core_is_network_reachable(mCore) == TRUE ? true : false;
}

void CoreAPI::setNetworkReachable(bool reachable) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setNetworkReachable()", "this=" << this << "\t" << "reachable=" << reachable);
	linphone_core_set_network_reachable(mCore, reachable ? TRUE : FALSE);
}

bool CoreAPI::audioAdaptiveJittcompEnabled() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::audioAdaptiveJittcompEnabled()", "this=" << this);
	return linphone_core_audio_adaptive_jittcomp_enabled(mCore) == TRUE ? true : false;
}

void CoreAPI::enableAudioAdaptiveJittcomp(bool enable) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableAudioAdaptiveJittcomp()", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_enable_audio_adaptive_jittcomp(mCore, enable ? TRUE : FALSE);
}

bool CoreAPI::videoAdaptiveJittcompEnabled() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::videoAdaptiveJittcompEnabled()", "this=" << this);
	return linphone_core_video_adaptive_jittcomp_enabled(mCore) == TRUE ? true : false;
}

void CoreAPI::enableVideoAdaptiveJittcomp(bool enable) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableVideoAdaptiveJittcomp()", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_enable_video_adaptive_jittcomp(mCore, enable ? TRUE : FALSE);
}

int CoreAPI::getAudioJittcomp() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getAudioJittcomp()", "this=" << this);
	return linphone_core_get_audio_jittcomp(mCore);
}

void CoreAPI::setAudioJittcomp(int comp) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setAudioJittcomp()", "this=" << this << "\t" << "comp=" << comp);
	linphone_core_set_audio_jittcomp(mCore, comp);
}

int CoreAPI::getVideoJittcomp() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getVideoJittcomp()", "this=" << this);
	return linphone_core_get_video_jittcomp(mCore);
}

void CoreAPI::setVideoJittcomp(int comp) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setVideoJittcomp()", "this=" << this << "\t" << "comp=" << comp);
	linphone_core_set_video_jittcomp(mCore, comp);
}

/*
 *
 * AuthInfo functions
 *
 */

void CoreAPI::addAuthInfo(const AuthInfoAPIPtr &authInfo) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::addAuthInfo()", "this=" << this << "\t" << "authInfo=" << authInfo);
	linphone_core_add_auth_info(mCore, authInfo->getRef());
}

void CoreAPI::abortAuthentication(const AuthInfoAPIPtr &authInfo) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::abortAuthentication()", "this=" << this << "\t" << "authInfo=" << authInfo);
	linphone_core_abort_authentication(mCore, authInfo->getRef());
}

void CoreAPI::removeAuthInfo(const AuthInfoAPIPtr &authInfo) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::removeAuthInfo()", "this=" << this << "\t" << "authInfo=" << authInfo);
	linphone_core_remove_auth_info(mCore, authInfo->getRef());
}

AuthInfoAPIPtr CoreAPI::findAuthInfo(const std::string &realm, const std::string &username) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::findAuthInfo()", "this=" << this << "\t" << "realm=" << realm << "\t" << "username=" << username);
	const LinphoneAuthInfo* authInfo = linphone_core_find_auth_info(mCore, realm.c_str(), username.c_str());
	return AuthInfoAPI::get(authInfo);
}

FB::VariantList CoreAPI::getAuthInfoList() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getAuthInfoList()", "this=" << this);
	FB::VariantList list;
	for (const MSList *node = linphone_core_get_auth_info_list(mCore); node != NULL; node = ms_list_next(node)) {
		list.push_back(AuthInfoAPI::get(reinterpret_cast<LinphoneAuthInfo*>(node->data)));
	}
	return list;
}

void CoreAPI::clearAllAuthInfo() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::clearAllAuthInfo()", "this=" << this);
	linphone_core_clear_all_auth_info(mCore);
}

/*
 *
 * Instantiator functions
 *
 */

ProxyConfigAPIPtr CoreAPI::newProxyConfig() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::newProxyConfig()", "this=" << this);
	return boost::make_shared<ProxyConfigAPI>();
}

AuthInfoAPIPtr CoreAPI::newAuthInfo(const std::string &username, const std::string &userid, const std::string &passwd, const std::string &ha1,
		const std::string &realm) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::newAuthInfo()", "this=" << this);
	return boost::make_shared<AuthInfoAPI>(username, userid, passwd, ha1, realm);
}


/*
 *
 * DTMF functions
 *
 */

void CoreAPI::sendDtmf(const std::string &dtmf) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::sendDtmf()", "this=" << this << "\t" << "dtmf=" << dtmf);
	if (dtmf.size() == 1)
		linphone_core_send_dtmf(mCore, dtmf[0]);
}

void CoreAPI::stopDtmf() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::playDtmf()", "this=" << this);
	linphone_core_stop_dtmf(mCore);
}

void CoreAPI::playDtmf(const std::string &dtmf, int duration_ms) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::playDtmf()", "this=" << this << "\t" << "dtmf="<< dtmf << ", duration_ms=" << duration_ms);
	if (dtmf.size() == 1)
		linphone_core_play_dtmf(mCore, dtmf[0], duration_ms);
}

bool CoreAPI::getUseInfoForDtmf() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getUseInfoForDtmf()", "this=" << this);
	return linphone_core_get_use_info_for_dtmf(mCore) == TRUE ? true : false;
}

void CoreAPI::setUseInfoForDtmf(bool enable) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setUseInfoForDtmf()", "this=" << this << "\t" << "enable="<< enable);
	linphone_core_set_use_info_for_dtmf(mCore, enable ? TRUE : FALSE);
}

bool CoreAPI::getUseRfc2833ForDtmf() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getUseRfc2833ForDtmf()", "this=" << this);
	return linphone_core_get_use_rfc2833_for_dtmf(mCore) == TRUE ? true : false;

}

void CoreAPI::setUseRfc2833ForDtmf(bool enable) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setUseRfc2833ForDtmf()", "this=" << this << "\t" << "enable="<< enable);
	linphone_core_set_use_rfc2833_for_dtmf(mCore, enable ? TRUE : FALSE);
}

/*
 *
 * Misc functions
 *
 */

std::string CoreAPI::getVersion() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getVersion()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_version());
}

std::string CoreAPI::getPluginVersion() const {
	FBLOG_DEBUG("CoreAPI::getPluginVersion()", "this=" << this << FBSTRING_PLUGIN_VERSION);
	return FBSTRING_PLUGIN_VERSION;
}

void CoreAPI::enableEchoCancellation(bool enable) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableEchoCancellation()", "this=" << this << "\t" << "enable="<< enable);

	linphone_core_enable_echo_cancellation(mCore, enable ? TRUE : FALSE);
}
bool CoreAPI::echoCancellationEnabled() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::echoCancellationEnabled()", "this=" << this);
	return linphone_core_echo_cancellation_enabled(mCore) == TRUE ? true : false;
}

void CoreAPI::enableEchoLimiter(bool enable) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableEchoLimiter()", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_enable_echo_limiter(mCore, enable ? TRUE : FALSE);
}

bool CoreAPI::echoLimiterEnabled() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::echoLimiterEnabled()", "this=" << this);
	return linphone_core_echo_limiter_enabled(mCore) == TRUE ? true : false;
}

void CoreAPI::setStaticPictureFps(float fps) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setStaticPictureFps()", "this=" << this << "\t" << "fps=" << fps);
	linphone_core_set_static_picture_fps(mCore, fps);
}

float CoreAPI::getStaticPictureFps() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getStaticPictureFps()", "this=" << this);
	return linphone_core_get_static_picture_fps(mCore);
}

/*
 *
 * File functions
 *
 */

std::string CoreAPI::getRing() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getRing()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_ring(mCore));
}

void CoreAPI::setRing(const std::string &ring) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setRing()", "this=" << this << "\t" << "ring=" << ring);
	linphone_core_set_ring(mCore, ring.c_str());
}

std::string CoreAPI::getRingback() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getRingback()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_ringback(mCore));
}

void CoreAPI::setRingback(const std::string &ringback) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setRingback()", "this=" << this << "\t" << "ringback=" << ringback);
	linphone_core_set_ringback(mCore, ringback.c_str());
}

std::string CoreAPI::getRootCa() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getRootCa()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_root_ca(mCore));
}

void CoreAPI::setRootCa(const std::string &rootCa) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setRootCa()", "this=" << this << "\t" << "rootCa=" << rootCa);
	linphone_core_set_root_ca(mCore, rootCa.c_str());
}

std::string CoreAPI::getStaticPicture() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getStaticPicture()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_static_picture(mCore));
}

void CoreAPI::setStaticPicture(const std::string &staticPicture) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setStaticPicture()", "this=" << this << "\t" << "staticPicture=" << staticPicture);
	linphone_core_set_static_picture(mCore, staticPicture.c_str());
}

std::string CoreAPI::getZrtpSecretsFile() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getZrtpSecretsFile()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_zrtp_secrets_file(mCore));
}

void CoreAPI::setZrtpSecretsFile(const std::string &secretsFile) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setZrtpSecretsFile()", "this=" << this << "\t" << "secretsFile=" << secretsFile);
	linphone_core_set_zrtp_secrets_file(mCore, secretsFile.c_str());
}

/*
 *
 * Download functions
 *
 */

void CoreAPI::download(const std::string& url, const FB::JSObjectPtr& callback) {
	FBLOG_DEBUG("CoreAPI::download()", "this=" << this << "\t" << "url=" << url);
	FB::URI uri = FB::URI(url);
	FB::SimpleStreamHelper::AsyncGet(m_host, uri, boost::bind(&CoreAPI::downloadCallback, this, uri, _1, _2, _3, _4, callback), boost::bind(&CoreAPI::downloadProgressCallback, this, uri, _1, _2, callback));
}

void CoreAPI::downloadCallback(const FB::URI& url, bool success, const FB::HeaderMap& headers, const boost::shared_array<uint8_t>& data,
		const size_t size, const FB::JSObjectPtr& callback) {
	if (success) {
		FBLOG_DEBUG("CoreAPI::downloadCallback()", "Download " << url.toString() << " success");

		// Build path
		boost::filesystem::path local = boost::filesystem::path(FB::System::getTempPath());
		local /= boost::filesystem::unique_path();
		FBLOG_DEBUG("CoreAPI::downloadCallback()", "Write to " << local.string());

		// Write to file
		boost::filesystem::ofstream downloadedFile(local, std::ios_base::binary | std::ios_base::out);
		downloadedFile.write(reinterpret_cast<const char*>(data.get()), size);
		if (downloadedFile.bad()) {
			downloadedFile.close();
			FBLOG_DEBUG("CoreAPI::downloadCallback()", "Can't write into file " << local.string());
			callback->InvokeAsync("", FB::variant_list_of(shared_from_this())(size)(size)(url.toString())("")("Can't write"));
			return;
		}
		downloadedFile.close();
		callback->InvokeAsync("", FB::variant_list_of(shared_from_this())(size)(size)(url.toString())(FILE_TO_URI(local.string()).toString())(""));
	} else {
		FBLOG_WARN("CoreAPI::downloadCallback()", "Download " << url.toString() << " failure");
		callback->InvokeAsync("", FB::variant_list_of(shared_from_this())(0)(0)(url.toString())("")("Can't download the file"));
	}
}

void CoreAPI::downloadProgressCallback(const FB::URI& url, const size_t received, const size_t total, const FB::JSObjectPtr& callback) {
	FBLOG_WARN("CoreAPI::downloadProgressCallback()", "Download " << url.toString() << " " << received << "/" << total);
	callback->InvokeAsync("", FB::variant_list_of(shared_from_this())(received)(total)(url.toString())("")(""));
}

///////////////////////////////////////////////////////////////////////////////
// Wrappers
///////////////////////////////////////////////////////////////////////////////

// Global callbacks which wraps CoreAPI object methods
#define GLC_DEFINED() (linphone_core_get_user_data(lc) != NULL)
#define GLC_THIS() boost::static_pointer_cast<CoreAPI>(((CoreAPI *)linphone_core_get_user_data(lc))->shared_from_this())
#define GLC(X) ((CoreAPI *)linphone_core_get_user_data(lc))->X

void CoreAPI::wrapper_global_state_changed(LinphoneCore *lc, LinphoneGlobalState gstate, const char *message) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "gstate = " << gstate << ", ";
		ss << "message = " << CHARPTR_TO_STRING(message);
		FBLOG_DEBUG("wrapper_global_state_changed", ss.str());
#endif //FB_NO_LOGGING_MACROS
		GLC(fire_globalStateChanged(GLC_THIS(), gstate, CHARPTR_TO_STRING(message)));
	}
}
void CoreAPI::wrapper_registration_state_changed(LinphoneCore *lc, LinphoneProxyConfig *cfg, LinphoneRegistrationState cstate, const char *message) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "cfg = " << cfg << ", ";
		ss << "cstate = " << cstate << ", ";
		ss << "message = " << CHARPTR_TO_STRING(message);
		FBLOG_DEBUG("wrapper_registration_state_changed", ss.str());
#endif //FB_NO_LOGGING_MACROS
		GLC(fire_registrationStateChanged(GLC_THIS(), ProxyConfigAPI::get(cfg), cstate, CHARPTR_TO_STRING(message)));
	}
}
void CoreAPI::wrapper_call_state_changed(LinphoneCore *lc, LinphoneCall *call, LinphoneCallState cstate, const char *message) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "call = " << call << ", ";
		ss << "cstate = " << cstate << ", ";
		ss << "message = " << CHARPTR_TO_STRING(message);
		FBLOG_DEBUG("wrapper_call_state_changed", ss.str());
#endif //FB_NO_LOGGING_MACROS
		GLC(fire_callStateChanged(GLC_THIS(), CallAPI::get(call), cstate, CHARPTR_TO_STRING(message)));
	}
}
void CoreAPI::wrapper_notify_presence_recv(LinphoneCore *lc, LinphoneFriend * lf) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "lf = " << lf;
		FBLOG_DEBUG("wrapper_notify_presence_recv", ss.str());
#endif //FB_NO_LOGGING_MACROS
	}
}
void CoreAPI::wrapper_new_subscription_request(LinphoneCore *lc, LinphoneFriend *lf, const char *url) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "lf = " << lf << ", ";
		ss << "url = " << url;
		FBLOG_DEBUG("wrapper_new_subscription_request", ss.str());
#endif //FB_NO_LOGGING_MACROS
	}
}
void CoreAPI::wrapper_auth_info_requested(LinphoneCore *lc, const char *realm, const char *username) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "realm = " << CHARPTR_TO_STRING(realm) << ", ";
		ss << "username = " << CHARPTR_TO_STRING(username);
		FBLOG_DEBUG("wrapper_auth_info_requested", ss.str());
		GLC(fire_authInfoRequested(GLC_THIS(), CHARPTR_TO_STRING(realm), CHARPTR_TO_STRING(username)));
#endif //FB_NO_LOGGING_MACROS
	}
}
void CoreAPI::wrapper_call_log_updated(LinphoneCore *lc, LinphoneCallLog *newcl) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "newcl = " << newcl;
		FBLOG_DEBUG("wrapper_call_log_updated", ss.str());
#endif //FB_NO_LOGGING_MACROS
	}
}
void CoreAPI::wrapper_text_received(LinphoneCore *lc, LinphoneChatRoom *room, const LinphoneAddress *from, const char *message) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "room = " << room << ", ";
		ss << "from = " << from << ", ";
		ss << "message = " << CHARPTR_TO_STRING(message);
		FBLOG_DEBUG("wrapper_text_received", ss.str());
#endif //FB_NO_LOGGING_MACROS
	}
}
void CoreAPI::wrapper_dtmf_received(LinphoneCore *lc, LinphoneCall *call, int dtmf) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "call = " << call << ", ";
		ss << "dtmf = " << dtmf;
		FBLOG_DEBUG("wrapper_dtmf_received", ss.str());
#endif //FB_NO_LOGGING_MACROS
	}
}
void CoreAPI::wrapper_refer_received(LinphoneCore *lc, const char *refer_to) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "refer_to = " << CHARPTR_TO_STRING(refer_to);
		FBLOG_DEBUG("wrapper_refer_received", ss.str());
#endif //FB_NO_LOGGING_MACROS
	}
}
void CoreAPI::wrapper_buddy_info_updated(LinphoneCore *lc, LinphoneFriend *lf) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "lf = " << lf;
		FBLOG_DEBUG("wrapper_buddy_info_updated", ss.str());
#endif //FB_NO_LOGGING_MACROS
	}
}
void CoreAPI::wrapper_notify_recv(LinphoneCore *lc, LinphoneCall *call, const char *from, const char *event) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "call = " << call << ", ";
		ss << "from = " << CHARPTR_TO_STRING(from) << ", ";
		ss << "event = " << event;
		FBLOG_DEBUG("wrapper_notify_recv", ss.str());
#endif //FB_NO_LOGGING_MACROS
	}
}
void CoreAPI::wrapper_display_status(LinphoneCore *lc, const char *message) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "message = " << CHARPTR_TO_STRING(message);
		FBLOG_DEBUG("wrapper_display_status", ss.str());
#endif //FB_NO_LOGGING_MACROS
		GLC(fire_displayStatus(GLC_THIS(), CHARPTR_TO_STRING(message)));
	}
}
void CoreAPI::wrapper_display_message(LinphoneCore *lc, const char *message) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "message = " << CHARPTR_TO_STRING(message);
		FBLOG_DEBUG("wrapper_display_message", ss.str());
#endif //FB_NO_LOGGING_MACROS
		GLC(fire_displayMessage(GLC_THIS(), CHARPTR_TO_STRING(message)));
	}
}
void CoreAPI::wrapper_display_warning(LinphoneCore *lc, const char *message) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "message = " << CHARPTR_TO_STRING(message);
		FBLOG_DEBUG("wrapper_display_warning", ss.str());
#endif //FB_NO_LOGGING_MACROS
		GLC(fire_displayWarning(GLC_THIS(), CHARPTR_TO_STRING(message)));
	}
}
void CoreAPI::wrapper_display_url(LinphoneCore *lc, const char *message, const char *url) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "message = " << CHARPTR_TO_STRING(message) << ", ";
		ss << "url = " << CHARPTR_TO_STRING(url);
		FBLOG_DEBUG("wrapper_display_url", ss.str());
#endif //FB_NO_LOGGING_MACROS
		GLC(fire_displayUrl(GLC_THIS(), CHARPTR_TO_STRING(message), CHARPTR_TO_STRING(url)));
	}
}
void CoreAPI::wrapper_show(LinphoneCore *lc) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		FBLOG_DEBUG("wrapper_show", ss.str());
#endif //FB_NO_LOGGING_MACROS
		GLC(fire_show(GLC_THIS()));
	}
}
void CoreAPI::wrapper_call_encryption_changed(LinphoneCore *lc, LinphoneCall *call, bool_t on, const char *authentication_token) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "call = " << call << ", ";
		ss << "on = " << on << ", ";
		ss << "authentication_token = " << authentication_token;
		FBLOG_DEBUG("wrapper_call_encryption_changed", ss.str());
#endif //FB_NO_LOGGING_MACROS
	}
}
