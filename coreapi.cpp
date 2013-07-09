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
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <sstream>

#include "factoryapi.h"
#include "coreplugin.h"

#include "addressapi.h"
#include "authinfoapi.h"
#include "callapi.h"
#include "calllogapi.h"
#include "callparamsapi.h"
#include "callstatsapi.h"
#include "coreapi.h"
#include "payloadtypeapi.h"
#include "proxyconfigapi.h"
#include "siptransportsapi.h"
#include "videopolicyapi.h"

namespace LinphoneWeb {

#ifndef WIN32
#else //WIN32
#include <windows.h>
void usleep(int waitTime) {
	Sleep(waitTime/1000);
}
#endif //WIN32

static boost::mutex sInstanceMutex;
static int sInstanceCount = 0;

#ifdef CORE_THREADED
#ifndef WIN32
#include <dlfcn.h>
#include <pthread.h>
#endif //WIN32

// Increment reference counter on this lib to avoid a early unload
void *CoreAPI::refLib() {
	void *libHandle = NULL;
#ifdef WIN32
	HMODULE module;
	if(GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<LPCTSTR>(&refLib), &module)) {
		libHandle = module;
	}
#else //WIN32
	Dl_info info;
	if(dladdr((void*)refLib, &info)) {
		libHandle = dlopen(info.dli_fname, RTLD_LAZY);
	}
#endif //WIN32
	return libHandle;
}

// Decrement reference counter on this lib
void CoreAPI::unrefLib(void *libHandle) {
#ifdef WIN32
	if(libHandle != NULL) {
		FreeLibraryAndExitThread((HMODULE)libHandle, 0);
	}
#else //WIN32
	if(libHandle != NULL) {
		pthread_cleanup_push((void(*)(void*))dlclose, (void*)libHandle);
		pthread_exit(0);
		pthread_cleanup_pop(0);
	}
#endif //WIN32
}

void CoreAPI::destroyThread(LinphoneCore *core, void *libHandle) {
	FBLOG_DEBUG("CoreAPI::destroyThread", "start" << "\t" << "core=" << core);
	linphone_core_destroy(core);
	sInstanceMutex.lock();
	--sInstanceCount;
	sInstanceMutex.unlock();
	FBLOG_DEBUG("CoreAPI::destroyThread", "end" << "\t" << "core=" << core);
	unrefLib(libHandle);
}

void CoreAPI::iterateThread(CoreAPIPtr &core) {
	FBLOG_DEBUG("CoreAPI::iterateThread", "start" << "\t" << "core=" << core);

	boost::this_thread::disable_interruption di;

	while (!boost::this_thread::interruption_requested()) {
		core->iterateWithMutex();
		usleep(core->mIterateInterval * 1000);
		//FBLOG_DEBUG("linphone_iterate_thread", "it" << "\t" << "core=" << core);
	}
	core->detachThread(boost::this_thread::get_id());
	core->mCoreThread.reset();
	FBLOG_DEBUG("CoreAPI::iterateThread", "end" << "\t" << "core=" << core);
}

void CoreAPI::iterateWithMutex() {
	CORE_MUTEX
	
	iterate();
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
CoreAPI::CoreAPI() :
		WrapperAPI(APIDescription(this)), mCore(NULL) {
	FBLOG_DEBUG("CoreAPI::CoreAPI", "this=" << this);
	
	mIterateInterval = 20;
	mIterate = false;
#ifdef CORE_THREADED
#else //CORE_THREADED
	mTimer = FB::Timer::getTimer(mIterateInterval, true, boost::bind(&CoreAPI::iterate, this));
#endif //CORE_THREADED
}

void CoreAPI::initProxy() {
	// Read-only property
	registerProperty("version", make_property(this, &CoreAPI::getVersion));
	registerProperty("pluginVersion", make_property(this, &CoreAPI::getPluginVersion));

	// Propery
	registerProperty("magic", make_property(this, &CoreAPI::getMagic, &CoreAPI::setMagic));
	
	// Core helpers
	registerMethod("init", make_method(this, &CoreAPI::init));
	registerMethod("uninit", make_method(this, &CoreAPI::uninit));
	registerProperty("iterateEnabled", make_property(this, &CoreAPI::iterateEnabled, &CoreAPI::enableIterate));
	registerProperty("iterateInterval", make_property(this, &CoreAPI::getIterateInterval, &CoreAPI::setIterateInterval));

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
	registerProperty("calls", make_property(this, &CoreAPI::getCalls));
	registerProperty("callsNb", make_property(this, &CoreAPI::getCallsNb));
	registerProperty("missedCallsCount", make_property(this, &CoreAPI::getMissedCallsCount));

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
	registerProperty("videoPolicy", make_property(this, &CoreAPI::getVideoPolicy, &CoreAPI::setVideoPolicy));

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

	// CallLog bindings
	registerMethod("clearCallLogs", make_method(this, &CoreAPI::clearCallLogs));
	registerProperty("callLogs", make_property(this, &CoreAPI::getCallLogs));
	
	// Network bindings
	registerProperty("audioPort", make_property(this, &CoreAPI::getAudioPort, &CoreAPI::setAudioPort));
	registerProperty("audioPortRange", make_property(this, &CoreAPI::getAudioPortRange, &CoreAPI::setAudioPortRange));
	registerProperty("videoPort", make_property(this, &CoreAPI::getVideoPort, &CoreAPI::setVideoPort));
	registerProperty("videoPortRange", make_property(this, &CoreAPI::getVideoPortRange, &CoreAPI::setVideoPortRange));
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
	registerProperty("sipTransports", make_property(this, &CoreAPI::getSipTransports, &CoreAPI::setSipTransports));
	registerProperty("adaptiveRateControl", make_property(this, &CoreAPI::adaptiveRateControlEnabled, &CoreAPI::enableAdaptiveRateControl));
	registerProperty("networkReachable", make_property(this, &CoreAPI::isNetworkReachable, &CoreAPI::setNetworkReachable));
	registerProperty("audioAdaptiveJittcomp", make_property(this, &CoreAPI::audioAdaptiveJittcompEnabled, &CoreAPI::enableAudioAdaptiveJittcomp));
	registerProperty("videoAdaptiveJittcomp", make_property(this, &CoreAPI::videoAdaptiveJittcompEnabled, &CoreAPI::enableVideoAdaptiveJittcomp));
	registerProperty("audioJittcomp", make_property(this, &CoreAPI::getAudioJittcomp, &CoreAPI::setAudioJittcomp));
	registerProperty("videoJittcomp", make_property(this, &CoreAPI::getVideoJittcomp, &CoreAPI::setVideoJittcomp));
	registerProperty("firewallPolicy", make_property(this, &CoreAPI::getFirewallPolicy, &CoreAPI::setFirewallPolicy));
	registerProperty("mediaEncryption", make_property(this, &CoreAPI::getMediaEncryption, &CoreAPI::setMediaEncryption));
	
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
	REGISTER_PROPERTY_FILE(CoreAPI, "playFile", getPlayFile, setPlayFile);
	REGISTER_PROPERTY_FILE(CoreAPI, "recordFile", getRecordFile, setRecordFile);
	REGISTER_PROPERTY_FILE(CoreAPI, "rootCa", getRootCa, setRootCa);
	REGISTER_PROPERTY_FILE(CoreAPI, "staticPicture", getStaticPicture, setStaticPicture);
	REGISTER_PROPERTY_FILE(CoreAPI, "zrtpSecretsFile", getZrtpSecretsFile, setZrtpSecretsFile);

	// Initiator bindings
	registerMethod("getFileManager", make_method(this, &CoreAPI::getFileManager));
	registerMethod("newProxyConfig", make_method(this, &CoreAPI::newProxyConfig));
	registerMethod("newAuthInfo", make_method(this, &CoreAPI::newAuthInfo));
	registerMethod("newAddress", make_method(this, &CoreAPI::newAddress));

	// Dtmf
	registerMethod("sendDtmf", make_method(this, &CoreAPI::sendDtmf));
	registerMethod("playDtmf", make_method(this, &CoreAPI::playDtmf));
	registerMethod("stopDtmf", make_method(this, &CoreAPI::stopDtmf));
	registerProperty("useInfoForDtmf", make_property(this, &CoreAPI::getUseInfoForDtmf, &CoreAPI::setUseInfoForDtmf));
	registerProperty("useRfc2833ForDtmf", make_property(this, &CoreAPI::getUseRfc2833ForDtmf, &CoreAPI::setUseRfc2833ForDtmf));

	// Presence
	registerProperty("presenceInfo", make_property(this, &CoreAPI::getPresenceInfo, &CoreAPI::setPresenceInfo));
	
	// Miscs
	registerProperty("echoCancellationEnabled", make_property(this, &CoreAPI::echoCancellationEnabled, &CoreAPI::enableEchoCancellation));
	registerProperty("echoLimiterEnabled", make_property(this, &CoreAPI::echoLimiterEnabled, &CoreAPI::enableEchoLimiter));
	registerProperty("staticPictureFps", make_property(this, &CoreAPI::getStaticPictureFps, &CoreAPI::setStaticPictureFps));
	registerProperty("userAgentName", make_property(this, &CoreAPI::getUserAgentName, &CoreAPI::setUserAgentName));
	registerProperty("userAgentVersion", make_property(this, &CoreAPI::getUserAgentVersion, &CoreAPI::setUserAgentVersion));
	registerMethod("interpretUrl", make_method(this, &CoreAPI::interpretUrl));
	
	// Log
	registerProperty("logHandler", make_property(this, &CoreAPI::getLogHandler, &CoreAPI::setLogHandler));
	
	// uPnP
	registerProperty("upnpAvailable", make_property(this, &CoreAPI::upnpAvailable));
	registerProperty("upnpExternalIpaddress", make_property(this, &CoreAPI::getUpnpExternalIpaddress));
	registerProperty("upnpState", make_property(this, &CoreAPI::getUpnpState));
	
	// Tunnel
	registerProperty("tunnelAvailable", make_property(this, &CoreAPI::tunnelAvailable));
}

int CoreAPI::init(const StringPtr &config, const StringPtr &factory) {
	FBLOG_DEBUG("CoreAPI::init", "this=" << this << "\t" << "config=" << config << "\t" << "factory=" << factory);
	StringPtr realConfig = config;
	StringPtr realFactory = factory;
	GET_FILE("CoreAPI::init", realConfig);
	GET_FILE("CoreAPI::init", realFactory);
	
	boost::mutex::scoped_lock scoped_instance_count_lock(sInstanceMutex);
	
	
#ifdef CORE_THREADED
	if(!mCoreThread) {
		mCoreThread = boost::make_shared<boost::thread>(CoreAPI::iterateThread, boost::static_pointer_cast<CoreAPI>(shared_from_this()));
		attachThread(mCoreThread);
	}
#else //CORE_THREADED
#endif //CORE_THREADED
	
	if(mCore != NULL) {
		return -1;
	}

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
		
		mCore = linphone_core_new(&mVtable, STRING_TO_CHARPTR(realConfig), STRING_TO_CHARPTR(realFactory), (void *) this);
		if (linphone_core_get_user_data(mCore) != this) {
			FBLOG_ERROR("CoreAPI::init", "Too old version of linphone core!");
			--sInstanceCount;
			return 1;
		}
		
		// Specific Linphone Web behaviour
		linphone_core_enable_video_preview(mCore, false); // MUST be disabled, we can't allow a detached window
		linphone_core_use_preview_window(mCore, false); // MUST be disabled, we can't allow a detached window

		return 0;
	} else {
		FBLOG_ERROR("CoreAPI::init", "Already started linphone instance");
		return 2;
	}
}


int CoreAPI::uninit() {
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::uninit", "this=" << this);
	if (mCore != NULL) {
		linphone_core_set_user_data(mCore, NULL);
		
#ifdef CORE_THREADED
		// TODO find a better way to do that
		boost::thread t(boost::bind(CoreAPI::destroyThread, mCore, refLib()));
#else  //CORE_THREADED
		mTimer->stop();
		linphone_core_destroy(mCore);
		sInstanceMutex.lock();
		--sInstanceCount;
		sInstanceMutex.unlock();
#endif //CORE_THREADED
		mCore = NULL;
		return 0;
	}
	return -1;
}

void CoreAPI::enableIterate(bool enable) {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::start", "this=" << this << "\t" << "enable=" << enable);
	
	if(enable == mIterate)
		return;
	
	if(enable) {
#ifdef CORE_THREADED
#else //CORE_THREADED
		mTimer->start();
#endif //CORE_THREADED
		mIterate = true;
	} else {
#ifdef CORE_THREADED
#else //CORE_THREADED
		mTimer->stop();
#endif //CORE_THREADED
		mIterate = false;
	}
}

bool CoreAPI::iterateEnabled() const {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::iterateEnabled", "this=" << this);
	return mIterate;
}

void CoreAPI::setIterateInterval(int ms) {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::setIterateInterval", "this=" << this << "\t" << "ms=" << ms);
	mIterateInterval = ms;
#ifdef CORE_THREADED
#else //CORE_THREADED
	mTimer->stop();
	mTimer = FB::Timer::getTimer(mIterateInterval, true, boost::bind(&CoreAPI::iterate, this));
	mTimer->start();
#endif //CORE_THREADED
}

int CoreAPI::getIterateInterval() const {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::getIterateInterval", "this=" << this);
	return mIterateInterval;
}

///////////////////////////////////////////////////////////////////////////////
/// @fn CoreAPI::~CoreAPI()
///
/// @brief  Destructor.  Remember that this object will not be released until
///         the browser is done with it; this will almost definitely be after
///         the plugin is released.
///////////////////////////////////////////////////////////////////////////////
CoreAPI::~CoreAPI() {
	FBLOG_DEBUG("CoreAPI::~CoreAPI", "this=" << this);
	uninit();
}

const StringPtr &CoreAPI::getMagic() const {
	FBLOG_DEBUG("CoreAPI::getMagic", "this=" << this);
	return mMagic;
}

void CoreAPI::setMagic(const StringPtr &magic) {
	FBLOG_DEBUG("CoreAPI::setMagic", "this=" << this << "\t" << "magic=" << magic);
	mMagic = magic;
}

/*
 *
 * Call functions
 *
 */

IMPLEMENT_SYNC_N_ASYNC(CoreAPI, invite, 1, (const StringPtr &), CallAPIPtr);

CallAPIPtr CoreAPI::invite(const StringPtr &url) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::invite", "this=" << this << "\t" << "url=" << url);
	LinphoneCall *call = linphone_core_invite(mCore, STRING_TO_CHARPTR(url));
	CallAPIPtr shared_call = getFactory()->getCall(call);
	return shared_call;
}

IMPLEMENT_SYNC_N_ASYNC(CoreAPI, inviteAddress, 1, (const AddressAPIPtr &), CallAPIPtr);

CallAPIPtr CoreAPI::inviteAddress(const AddressAPIPtr &address) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::inviteAddress", "this=" << this << "\t" << "address=" << address);
	LinphoneCall *call = linphone_core_invite_address(mCore, address->getRef());
	CallAPIPtr shared_call = getFactory()->getCall(call);
	return shared_call;
}

IMPLEMENT_SYNC_N_ASYNC(CoreAPI, inviteWithParams, 2, (const StringPtr &, const CallParamsAPIPtr &), CallAPIPtr);

CallAPIPtr CoreAPI::inviteWithParams(const StringPtr &url, const CallParamsAPIPtr &params) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::invite", "this=" << this << "\t" << "url=" << url << "\t" << "params=" << params);
	LinphoneCall *call = linphone_core_invite_with_params(mCore, STRING_TO_CHARPTR(url), params->getRef());
	CallAPIPtr shared_call = getFactory()->getCall(call);
	return shared_call;
}

IMPLEMENT_SYNC_N_ASYNC(CoreAPI, inviteAddressWithParams, 2, (const AddressAPIPtr &, const CallParamsAPIPtr &), CallAPIPtr);

CallAPIPtr CoreAPI::inviteAddressWithParams(const AddressAPIPtr &address, const CallParamsAPIPtr &params) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::inviteAddress", "this=" << this << "\t" << "address=" << address << "\t" << "params=" << params);
	LinphoneCall *call = linphone_core_invite_address_with_params(mCore, address->getRef(), params->getRef());
	CallAPIPtr shared_call = getFactory()->getCall(call);
	return shared_call;
}

int CoreAPI::acceptCall(const CallAPIPtr &call) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::acceptCall", "this=" << this << "\t" << "call=" << call);
	return linphone_core_accept_call(mCore, call->getRef());
}

int CoreAPI::acceptCallWithParams(const CallAPIPtr &call, const CallParamsAPIPtr &params) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::acceptCallWithParams", "this=" << this << "\t" << "call=" << call << "\t" << "params=" << params);
	return linphone_core_accept_call_with_params(mCore, call->getRef(), params->getRef());
}

CallAPIPtr CoreAPI::getCurrentCall() {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getCurrentCall", "this=" << this);
	return getFactory()->getCall(linphone_core_get_current_call(mCore));
}

int CoreAPI::terminateCall(const CallAPIPtr &call) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::terminateCall", "this=" << this << "\t" << "call=" << call);
	return linphone_core_terminate_call(mCore, call->getRef());
}

int CoreAPI::terminateAllCalls() {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::terminateAllCalls", "this=" << this);
	return linphone_core_terminate_all_calls(mCore); 
}

int CoreAPI::redirectCall(const CallAPIPtr &call, const StringPtr &uri) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::redirectCall", "this=" << this << "\t" << "call=" << call << "\t" << "uri=" << uri);
	return linphone_core_redirect_call(mCore, call->getRef(), STRING_TO_CHARPTR(uri));
}

int CoreAPI::declineCall(const CallAPIPtr &call, int reason) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::declineCall", "this=" << this << "\t" << "call=" << call << "\t" << "reason=" << reason);
	return linphone_core_decline_call(mCore, call->getRef(), (LinphoneReason)reason);
}

int CoreAPI::transferCall(const CallAPIPtr &call, const StringPtr &uri) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::transferCall", "this=" << this << "\t" << "call=" << call << "\t" << "uri=" << uri);
	return linphone_core_transfer_call(mCore, call->getRef(), STRING_TO_CHARPTR(uri));
}

int CoreAPI::transferCallToAnother(const CallAPIPtr &call, const CallAPIPtr &dest) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::transferCallToAnother", "this=" << this << "\t" << "call=" << call << "\t" << "dest=" << dest);
	return linphone_core_transfer_call_to_another(mCore, call->getRef(), dest->getRef());
}

int CoreAPI::resumeCall(const CallAPIPtr &call) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::resumeCall", "this=" << this << "\t" << "call=" << call);
	return linphone_core_resume_call(mCore, call->getRef());
}

int CoreAPI::pauseCall(const CallAPIPtr &call) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::pauseCall", "this=" << this << "\t" << "call=" << call);
	return linphone_core_pause_call(mCore, call->getRef());
}

int CoreAPI::pauseAllCalls() {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::pauseAllCalls", "this=" << this);
	return linphone_core_pause_all_calls(mCore);
}

int CoreAPI::updateCall(const CallAPIPtr &call, const CallParamsAPIPtr &params) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::updateCall", "this=" << this << "\t" << "call=" << call << "\t" << "params=" << params);
	return linphone_core_update_call(mCore, call->getRef(), params->getRef());
}

int CoreAPI::deferCallUpdate(const CallAPIPtr &call) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::deferCallUpdate", "this=" << this << "\t" << "call=" << call);
	return linphone_core_defer_call_update(mCore, call->getRef());
}

int CoreAPI::acceptCallUpdate(const CallAPIPtr &call, const CallParamsAPIPtr &params) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::acceptCallUpdate", "this=" << this << "\t" << "call=" << call << "\t" << "params=" << params);
	return linphone_core_accept_call_update(mCore, call->getRef(), params->getRef());
}

CallParamsAPIPtr CoreAPI::createDefaultCallParameters() {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::createDefaultCallParameters", "this=" << this);
	return getFactory()->getCallParams(linphone_core_create_default_call_parameters(mCore));
}

void CoreAPI::setIncTimeout(int timeout) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setIncTimeout", "this=" << this << "\t" << "timeout=" << timeout);
	linphone_core_set_inc_timeout(mCore, timeout);
}

int CoreAPI::getIncTimeout() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getIncTimeout", "this=" << this);
	return linphone_core_get_inc_timeout(mCore);
}

void CoreAPI::setInCallTimeout(int timeout) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setInCallTimeout", "this=" << this << "\t" << "timeout=" << timeout);
	linphone_core_set_in_call_timeout(mCore, timeout);
}

int CoreAPI::getInCallTimeout() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getInCallTimeout", "this=" << this);
	return linphone_core_get_in_call_timeout(mCore);
}

int CoreAPI::getMaxCalls() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getMaxCalls", "this=" << this);
	return linphone_core_get_max_calls(mCore);
}

void CoreAPI::setMaxCalls(int max) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setMaxCalls", "this=" << this << "\t" << "max=" << max);
	linphone_core_set_max_calls(mCore, max);
}
	
std::vector<CallAPIPtr> CoreAPI::getCalls() const {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::getCalls", "this=" << this);
	std::vector<CallAPIPtr> list;
	for (const MSList *node = linphone_core_get_calls(mCore); node != NULL; node = ms_list_next(node)) {
		list.push_back(getFactory()->getCall(reinterpret_cast<LinphoneCall *>(node->data)));
	}
	return list;
}

int CoreAPI::getCallsNb() const {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::getCallsNb", "this=" << this);
	return linphone_core_get_calls_nb(mCore);
}

int CoreAPI::getMissedCallsCount() const {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::getMissedCallsCount", "this=" << this);
	return linphone_core_get_missed_calls_count(mCore);
}
	
/*
 *
 * Conference functions
 *
 */

int CoreAPI::addAllToConference() {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::addAllToConference", "this=" << this);
	return linphone_core_add_all_to_conference(mCore);
}

int CoreAPI::addToConference(const CallAPIPtr &call) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::addToConference", "this=" << this << "\t" << "call=" << call);
	return linphone_core_add_to_conference(mCore, call->getRef());
}

int CoreAPI::enterConference() {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enterConference", "this=" << this);
	return linphone_core_enter_conference(mCore);
}

int CoreAPI::getConferenceSize() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getConferenceSize", "this=" << this);
	return linphone_core_get_conference_size(mCore);
}

bool CoreAPI::isInConference() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::isInConference", "this=" << this);
	return linphone_core_is_in_conference(mCore) == TRUE ? true : false;
}

int CoreAPI::leaveConference() {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::leaveConference", "this=" << this);
	return linphone_core_leave_conference(mCore);
}

int CoreAPI::removeFromConference(const CallAPIPtr &call) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::removeFromConference", "this=" << this << "\t" << "call=" << call);
	return linphone_core_remove_from_conference(mCore, call->getRef());
}

int CoreAPI::terminateConference() {
	FB_ASSERT_CORE
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
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setPlayLevel", "this=" << this << "\t" << "level=" << level);
	linphone_core_set_play_level(mCore, level);
}

int CoreAPI::getPlayLevel() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getPlayLevel", "this=" << this);
	return linphone_core_get_play_level(mCore);
}

void CoreAPI::setRecLevel(int level) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setRecLevel", "this=" << this << "\t" << "level=" << level);
	linphone_core_set_rec_level(mCore, level);
}

int CoreAPI::getRecLevel() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getRecLevel", "this=" << this);
	return linphone_core_get_rec_level(mCore);
}

void CoreAPI::setRingLevel(int level) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setRingLevel", "this=" << this << "\t" << "level=" << level);
	linphone_core_set_ring_level(mCore, level);
}

int CoreAPI::getRingLevel() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getRingLevel", "this=" << this);
	return linphone_core_get_ring_level(mCore);
}

void CoreAPI::muteMic(bool muted) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::muteMic", "this=" << this << "\t" << "muted=" << muted);
	linphone_core_mute_mic(mCore, muted ? TRUE : FALSE);
}

bool CoreAPI::isMicMuted() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::isMicMuted", "this=" << this);
	return linphone_core_is_mic_muted(mCore) == TRUE ? true : false;
}

float CoreAPI::getMicGainDb() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getMicGainDb", "this=" << this);
	return linphone_core_get_mic_gain_db(mCore);
}

void CoreAPI::setMicGainDb(float gain) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setMicGainDb", "this=" << this << "\t" << "gain=" << gain);
	return linphone_core_set_mic_gain_db(mCore, gain);
}

float CoreAPI::getPlaybackGainDb() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getPlaybackGainDb", "this=" << this);
	return linphone_core_get_playback_gain_db(mCore);
}

void CoreAPI::setPlaybackGainDb(float gain) {
	FB_ASSERT_CORE
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
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::videoSupported", "this=" << this);
	return linphone_core_video_supported(mCore) == TRUE ? true : false;
}
void CoreAPI::enableVideo(bool enable) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableVideo", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_enable_video(mCore, enable ? TRUE : FALSE, enable ? TRUE : FALSE);
}

bool CoreAPI::videoEnabled() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::videoEnabled", "this=" << this);
	return linphone_core_video_enabled(mCore) == TRUE ? true : false;
}

void CoreAPI::enableVideoPreview(bool enable) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableVideoPreview", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_enable_video_preview(mCore, enable ? TRUE : FALSE);
}

bool CoreAPI::videoPreviewEnabled() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::videoPreviewEnabled", "this=" << this);
	return linphone_core_video_preview_enabled(mCore) == TRUE ? true : false;
}

void CoreAPI::enableSelfView(bool enable) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableSelfView", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_enable_self_view(mCore, enable ? TRUE : FALSE);
}

bool CoreAPI::selfViewEnabled() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::selfViewEnabled", "this=" << this);
	return linphone_core_self_view_enabled(mCore) == TRUE ? true : false;
}

void CoreAPI::setNativeVideoWindowId(unsigned long id) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setNativeVideoWindowId", "this=" << this << "\t" << "id=" << id);
	linphone_core_set_native_video_window_id(mCore, id);
}

unsigned long CoreAPI::getNativeVideoWindowId() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getNativeVideoWindowId", "this=" << this);
	return linphone_core_get_native_video_window_id(mCore);
}

void CoreAPI::setNativePreviewWindowId(unsigned long id) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setNativePreviewWindowId", "this=" << this << "\t" << "id=" << id);
	linphone_core_set_native_preview_window_id(mCore, id);
}

unsigned long CoreAPI::getNativePreviewWindowId() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getNativePreviewWindowId", "this=" << this);
	return linphone_core_get_native_preview_window_id(mCore);
}

bool CoreAPI::getUsePreviewWindow() const {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::getUsePreviewWindow", "this=" << this);
	FBLOG_ERROR("CoreAPI::getUsePreviewWindow", "NOT IMPLEMENTED");
	return FALSE; // TODO Don't have API yet
}

void CoreAPI::setUsePreviewWindow(bool enable) {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::setUsePreviewWindow", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_use_preview_window(mCore, enable);
}

VideoPolicyAPIPtr CoreAPI::getVideoPolicy() const {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::getVideoPolicy", "this=" << this);
	VideoPolicyAPIPtr videoPolicy = getFactory()->getVideoPolicy();
	*videoPolicy->getRef() = *linphone_core_get_video_policy(mCore);
	return videoPolicy;
}

void CoreAPI::setVideoPolicy(const VideoPolicyAPIPtr &videoPolicy) {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::setVideoPolicy", "this=" << this << "\t" << "policy=" << videoPolicy);
	linphone_core_set_video_policy(mCore, videoPolicy->getRef());
}

/*
 *
 * Sound device functions
 *
 */

void CoreAPI::reloadSoundDevices() {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::reloadSoundDevices", "this=" << this);

	linphone_core_reload_sound_devices(mCore);
}

std::vector<StringPtr> CoreAPI::getSoundDevices() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getSoundDevices", "this=" << this);
	std::vector<StringPtr> list;
	const char **devlist = linphone_core_get_sound_devices(mCore);
	while (devlist != NULL && *devlist != NULL) {
		list.push_back(StringPtr(*devlist++));
	}

	return list;
}

bool CoreAPI::soundDeviceCanCapture(const StringPtr &devid) const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::soundDeviceCanCapture", "this=" << this << "\t" << "devid=" << devid);
	return linphone_core_sound_device_can_capture(mCore, STRING_TO_CHARPTR(devid)) == TRUE ? true : false;
}

bool CoreAPI::soundDeviceCanPlayback(const StringPtr &devid) const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::soundDeviceCanPlayback", "this=" << this << "\t" << "devid=" << devid);
	return linphone_core_sound_device_can_playback(mCore, STRING_TO_CHARPTR(devid)) == TRUE ? true : false;
}

void CoreAPI::setRingerDevice(const StringPtr &devid) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setRingerDevice", "this=" << this << "\t" << "devid=" << devid);
	linphone_core_set_ringer_device(mCore, STRING_TO_CHARPTR(devid));
}

void CoreAPI::setPlaybackDevice(const StringPtr &devid) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setPlaybackDevice", "this=" << this << "\t" << "devid=" << devid);
	linphone_core_set_playback_device(mCore, STRING_TO_CHARPTR(devid));
}

void CoreAPI::setCaptureDevice(const StringPtr &devid) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setCaptureDevice", "this=" << this << "\t" << "devid=" << devid);
	linphone_core_set_capture_device(mCore, STRING_TO_CHARPTR(devid));
}

StringPtr CoreAPI::getRingerDevice() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getRingerDevice", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_ringer_device(mCore));
}

StringPtr CoreAPI::getPlaybackDevice() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getPlaybackDevice", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_playback_device(mCore));
}

StringPtr CoreAPI::getCaptureDevice() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getPlaybackDevice", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_capture_device(mCore));
}

/*
 *
 * Video device functions
 *
 */

void CoreAPI::reloadVideoDevices() {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::reloadVideoDevices", "this=" << this);

	linphone_core_reload_video_devices(mCore);
}

std::vector<StringPtr> CoreAPI::getVideoDevices() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getVideoDevices", "this=" << this);

	std::vector<StringPtr> list;
	const char **devlist = linphone_core_get_video_devices(mCore);
	while (devlist != NULL && *devlist != NULL) {
		list.push_back(StringPtr(*devlist++));
	}

	return list;
}

void CoreAPI::setVideoDevice(const StringPtr &devid) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setVideoDevice", "this=" << this << "\t" << "devid=" << devid);
	linphone_core_set_video_device(mCore, STRING_TO_CHARPTR(devid));
}

StringPtr CoreAPI::getVideoDevice() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getVideoDevice", "this=" << this);

	return CHARPTR_TO_STRING(linphone_core_get_video_device(mCore));
}

/*
 *
 * Codecs functions
 *
 */

std::vector<PayloadTypeAPIPtr> CoreAPI::getAudioCodecs() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getAudioCodecs", "this=" << this);
	std::vector<PayloadTypeAPIPtr> list;
	for (const MSList *node = linphone_core_get_audio_codecs(mCore); node != NULL; node = ms_list_next(node)) {
		list.push_back(getFactory()->getPayloadType(reinterpret_cast< ::PayloadType*>(node->data)));
	}
	return list;
}

std::vector<PayloadTypeAPIPtr> CoreAPI::getVideoCodecs() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getVideoCodecs", "this=" << this);
	std::vector<PayloadTypeAPIPtr> list;
	for (const MSList *node = linphone_core_get_video_codecs(mCore); node != NULL; node = ms_list_next(node)) {
		list.push_back(getFactory()->getPayloadType(reinterpret_cast< ::PayloadType*>(node->data)));
	}
	return list;
}

void CoreAPI::setAudioCodecs(const std::vector<PayloadTypeAPIPtr> &list) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::set_audio_codecs", "this=" << this << "\t" << "list.size()=" << list.size());
	MSList *mslist = NULL;
	for (auto it = list.begin(); it != list.end(); ++it) {
			mslist = ms_list_append(mslist, (*it)->getRef());
	}

	linphone_core_set_audio_codecs(mCore, mslist);
}

void CoreAPI::setVideoCodecs(const std::vector<PayloadTypeAPIPtr> &list) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setVideoCodecs", "this=" << this << "\t" << "list.size()=" << list.size());
	MSList *mslist = NULL;
	for (auto it = list.begin(); it != list.end(); ++it) {
		mslist = ms_list_append(mslist, (*it)->getRef());
	}

	linphone_core_set_video_codecs(mCore, mslist);
}

bool CoreAPI::payloadTypeEnabled(const PayloadTypeAPIPtr &payloadType) const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::payloadTypeEnabled", "this=" << this << "\t" << "payloadType=" << payloadType);
	return linphone_core_payload_type_enabled(mCore, payloadType->getRef()) == TRUE ? true : false;
}

void CoreAPI::enablePayloadType(const PayloadTypeAPIPtr &payloadType, bool enable) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enablePayloadType", "this=" << this << "\t" << "payloadType=" << payloadType << "\t" << "enable=" << enable);
	linphone_core_enable_payload_type(mCore, payloadType->getRef(), enable ? TRUE : FALSE);
}

/*
 *
 * Proxy functions
 *
 */

int CoreAPI::addProxyConfig(const ProxyConfigAPIPtr &config) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::addProxyConfig", "this=" << this << "\t" << "config=" << config);
	config->disOwn();
	return linphone_core_add_proxy_config(mCore, config->getRef());
}

void CoreAPI::clearProxyConfig() {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::clearProxyConfig", "this=" << this);
	linphone_core_clear_proxy_config(mCore);
}

void CoreAPI::removeProxyConfig(const ProxyConfigAPIPtr &config) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::removeProxyConfig", "this=" << this << "\t" << "config=" << config);
	config->disOwn();
	linphone_core_remove_proxy_config(mCore, config->getRef());
}

std::vector<ProxyConfigAPIPtr> CoreAPI::getProxyConfigList() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getProxyConfigList", "this=" << this);
	std::vector<ProxyConfigAPIPtr> list;
	for (const MSList *node = linphone_core_get_proxy_config_list(mCore); node != NULL; node = ms_list_next(node)) {
		list.push_back(getFactory()->getProxyConfig(reinterpret_cast<LinphoneProxyConfig*>(node->data)));
	}
	return list;
}

void CoreAPI::setDefaultProxy(const ProxyConfigAPIPtr &config) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setDefaultProxy", "this=" << this << "\t" << "config=" << config);
	config->disOwn();
	linphone_core_set_default_proxy(mCore, config->getRef());
}

ProxyConfigAPIPtr CoreAPI::getDefaultProxy() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getDefaultProxy", "this=" << this);
	LinphoneProxyConfig *ptr = NULL;
	linphone_core_get_default_proxy(mCore, &ptr);
	if (ptr != NULL)
		return getFactory()->getProxyConfig(ptr);
	return ProxyConfigAPIPtr();
}

void CoreAPI::setPrimaryContact(const StringPtr &contact) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setPrimaryContact", "this=" << this << "\t" << "contact=" << contact);
	linphone_core_set_primary_contact(mCore, STRING_TO_CHARPTR(contact));
}

StringPtr CoreAPI::getPrimaryContact() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getPrimaryContact", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_primary_contact(mCore));
}

void CoreAPI::refreshRegisters() {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::refreshRegisters", "this=" << this);
	linphone_core_refresh_registers(mCore);
}

	
/*
 *
 * CallLog functions
 *
 */
	
void CoreAPI::clearCallLogs() {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::clearCallLogs", "this=" << this);
	linphone_core_clear_call_logs(mCore);
}
	
std::vector<CallLogAPIPtr> CoreAPI::getCallLogs() const {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::getCallLogs", "this=" << this);
	std::vector<CallLogAPIPtr> list;
	for (const MSList *node = linphone_core_get_call_logs(mCore); node != NULL; node = ms_list_next(node)) {
		list.push_back(getFactory()->getCallLog(reinterpret_cast<LinphoneCallLog *>(node->data)));
	}
	return list;
}
	
/*
 *
 * Network functions
 *
 */

static StringPtr printRange(int min, int max) {
	std::stringstream ss;
	ss << min  << ":" << max;
	return ss.str();
}

static bool parseRange(const std::string &str, int &min, int &max) {
	std::vector<std::string> tokens;
	boost::split(tokens, str, boost::is_any_of(":"));
	if(tokens.size() != 2) {
		return false;
	}
	
	try {
		min = boost::lexical_cast<int>(tokens[0]);
		max = boost::lexical_cast<int>(tokens[1]);
	} catch(boost::bad_lexical_cast &) {
		return false;
	}
	
	if(max < min) {
		return false;
	}
	return true;
}

static bool validPort(int port) {
	if(port < 0) {
        return false;
    }
    if(port > 65535) {
        return false;
    }
	return true;
}

void CoreAPI::setAudioPort(int port) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setAudioPort", "this=" << this << "\t" << "port=" << port);
	linphone_core_set_audio_port(mCore, port);
}

int CoreAPI::getAudioPort() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getAudioPort", "this=" << this);
	return linphone_core_get_audio_port(mCore);
}

void CoreAPI::setAudioPortRange(const StringPtr &range) {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::setAudioPortRange", "this=" << this << "\t" << "range=" << range);
	int min, max;
	if(range && parseRange(*range, min, max) && validPort(min) && validPort(max)) {
		linphone_core_set_audio_port_range(mCore, min, max);
	} else {
		FBLOG_WARN("CoreAPI::setAudioPortRange", "Invalid port range: " << range);
		throw FB::script_error(std::string("Invalid port range: ") + PRINT_STRING(range));
	}
}

StringPtr CoreAPI::getAudioPortRange() const {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::getAudioPortRange", "this=" << this);
	int min, max;
	linphone_core_get_audio_port_range(mCore, &min, &max);
	return printRange(min, max);
}

void CoreAPI::setVideoPort(int port) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setVideoPort", "this=" << this << "\t" << "port=" << port);
	linphone_core_set_video_port(mCore, port);
}

int CoreAPI::getVideoPort() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getVideoPort", "this=" << this);
	return linphone_core_get_video_port(mCore);
}

void CoreAPI::setVideoPortRange(const StringPtr &range) {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::setVideoPortRange", "this=" << this << "\t" << "range=" << range);
	int min, max;
	if(range && parseRange(*range, min, max) && validPort(min) && validPort(max)) {
		linphone_core_set_video_port_range(mCore, min, max);
	} else {
		FBLOG_WARN("CoreAPI::setVideoPortRange", "Invalid port range: " << range);
		throw FB::script_error(std::string("Invalid port range: ") + PRINT_STRING(range));
	}
}

StringPtr CoreAPI::getVideoPortRange() const {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::getVideoPortRange", "this=" << this);
	int min, max;
	linphone_core_get_video_port_range(mCore, &min, &max);
	return printRange(min, max);
}

void CoreAPI::setDownloadBandwidth(int bandwidth) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setDownloadBandwidth", "this=" << this << "\t" << "bandwidth=" << bandwidth);
	linphone_core_set_download_bandwidth(mCore, bandwidth);
}

int CoreAPI::getDownloadBandwidth() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getDownloadBandwidth", "this=" << this);
	return linphone_core_get_download_bandwidth(mCore);
}

void CoreAPI::setUploadBandwidth(int bandwidth) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setUploadBandwidth", "this=" << this << "\t" << "bandwidth=" << bandwidth);
	linphone_core_set_upload_bandwidth(mCore, bandwidth);
}

int CoreAPI::getUploadBandwidth() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getUploadBandwidth", "this=" << this);
	return linphone_core_get_upload_bandwidth(mCore);
}

void CoreAPI::setDownloadPtime(int ptime) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setDownloadPtime", "this=" << this << "\t" << "ptime=" << ptime);
	linphone_core_set_download_ptime(mCore, ptime);
}

int CoreAPI::getDownloadPtime() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getDownloadPtime", "this=" << this);
	return linphone_core_get_download_ptime(mCore);
}

void CoreAPI::setUploadPtime(int ptime) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setUploadPtime", "this=" << this << "\t" << "ptime=" << ptime);
	linphone_core_set_upload_ptime(mCore, ptime);
}

int CoreAPI::getUploadPtime() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getUploadPtime", "this=" << this);
	return linphone_core_get_upload_ptime(mCore);
}

void CoreAPI::setMtu(int mtu) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setMtu", "this=" << this << "\t" << "mtu=" << mtu);
	return linphone_core_set_mtu(mCore, mtu);
}

int CoreAPI::getMtu() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getMtu", "this=" << this);
	return linphone_core_get_mtu(mCore);
}

void CoreAPI::setStunServer(const StringPtr &server) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setStunServer", "this=" << this << "\t" << "server=" << server);
	return linphone_core_set_stun_server(mCore, STRING_TO_CHARPTR(server));
}

StringPtr CoreAPI::getStunServer() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getStunServer", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_stun_server(mCore));
}

void CoreAPI::setRelayAddr(const StringPtr &addr) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setRelayAddr", "this=" << this << "\t" << "addr=" << addr);
	linphone_core_set_relay_addr(mCore, STRING_TO_CHARPTR(addr));
}

StringPtr CoreAPI::getRelayAddr() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getRelayAddr", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_relay_addr(mCore));
}

void CoreAPI::setNatAddress(const StringPtr &address) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setNatAddress", "this=" << this << "\t" << "address=" << address);
	return linphone_core_set_nat_address(mCore, STRING_TO_CHARPTR(address));
}

StringPtr CoreAPI::getNatAddress() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getNatAddress", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_nat_address(mCore));
}

void CoreAPI::setGuessHostname(bool guess) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setGuessHostname", "this=" << this << "\t" << "guess=" << guess);
	return linphone_core_set_guess_hostname(mCore, guess ? TRUE : FALSE);
}

bool CoreAPI::getGuessHostname() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getGuessHostname", "this=" << this);
	return linphone_core_get_guess_hostname(mCore) == TRUE ? true : false;
}

void CoreAPI::enableIpv6(bool enable) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableIpv6", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_enable_ipv6(mCore, enable ? TRUE : FALSE);
}

bool CoreAPI::ipv6Enabled() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::ipv6Enabled", "this=" << this);
	return linphone_core_ipv6_enabled(mCore) == TRUE ? true : false;
}

void CoreAPI::enableKeepAlive(bool enable) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableKeepAlive", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_enable_keep_alive(mCore, enable ? TRUE : FALSE);
}

bool CoreAPI::keepAliveEnabled() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::keepAliveEnabled", "this=" << this);
	return linphone_core_keep_alive_enabled(mCore) == TRUE ? true : false;
}

void CoreAPI::setAudioDscp(int dscp) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setAudioDscp", "this=" << this << "\t" << "dscp=" << dscp);
	linphone_core_set_audio_dscp(mCore, dscp);
}

int CoreAPI::getAudioDscp() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getAudioDscp", "this=" << this);
	return linphone_core_get_audio_dscp(mCore);
}

void CoreAPI::setSipDscp(int dscp) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setSipDscp", "this=" << this << "\t" << "dscp=" << dscp);
	linphone_core_set_sip_dscp(mCore, dscp);
}

int CoreAPI::getSipDscp() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getSipDscp", "this=" << this);
	return linphone_core_get_sip_dscp(mCore);
}

void CoreAPI::setVideoDscp(int dscp) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setVideoDscp", "this=" << this << "\t" << "dscp=" << dscp);
	linphone_core_set_video_dscp(mCore, dscp);
}

int CoreAPI::getVideoDscp() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getVideoDscp", "this=" << this);
	return linphone_core_get_video_dscp(mCore);
}

void CoreAPI::setSipPort(int port) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setSipPort", "this=" << this << "\t" << "port=" << port);
	linphone_core_set_sip_port(mCore, port);
}

int CoreAPI::getSipPort() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getSipPort", "this=" << this);
	return linphone_core_get_sip_port(mCore);
}

void CoreAPI::setSipTransports(const SipTransportsAPIPtr &sipTransports) {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::setSipTransports", "this=" << this << "\t" << "sipTransports=" << sipTransports);
	linphone_core_set_sip_transports(mCore, sipTransports->getRef());
}

SipTransportsAPIPtr CoreAPI::getSipTransports() const {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::getSipTransports", "this=" << this);
	SipTransportsAPIPtr sipTransports = getFactory()->getSipTransports();
	linphone_core_get_sip_transports(mCore, sipTransports->getRef());
	return sipTransports;
}

bool CoreAPI::adaptiveRateControlEnabled() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::adaptiveRateControlEnabled", "this=" << this);
	return linphone_core_adaptive_rate_control_enabled(mCore) == TRUE ? true : false;
}

void CoreAPI::enableAdaptiveRateControl(bool enable) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableAdaptiveRateControl", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_enable_adaptive_rate_control(mCore, enable ? TRUE : FALSE);
}

bool CoreAPI::isNetworkReachable() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::isNetworkReachable", "this=" << this);
	return linphone_core_is_network_reachable(mCore) == TRUE ? true : false;
}

void CoreAPI::setNetworkReachable(bool reachable) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setNetworkReachable", "this=" << this << "\t" << "reachable=" << reachable);
	linphone_core_set_network_reachable(mCore, reachable ? TRUE : FALSE);
}

bool CoreAPI::audioAdaptiveJittcompEnabled() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::audioAdaptiveJittcompEnabled", "this=" << this);
	return linphone_core_audio_adaptive_jittcomp_enabled(mCore) == TRUE ? true : false;
}

void CoreAPI::enableAudioAdaptiveJittcomp(bool enable) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableAudioAdaptiveJittcomp", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_enable_audio_adaptive_jittcomp(mCore, enable ? TRUE : FALSE);
}

bool CoreAPI::videoAdaptiveJittcompEnabled() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::videoAdaptiveJittcompEnabled", "this=" << this);
	return linphone_core_video_adaptive_jittcomp_enabled(mCore) == TRUE ? true : false;
}

void CoreAPI::enableVideoAdaptiveJittcomp(bool enable) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableVideoAdaptiveJittcomp", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_enable_video_adaptive_jittcomp(mCore, enable ? TRUE : FALSE);
}

int CoreAPI::getAudioJittcomp() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getAudioJittcomp", "this=" << this);
	return linphone_core_get_audio_jittcomp(mCore);
}

void CoreAPI::setAudioJittcomp(int comp) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setAudioJittcomp", "this=" << this << "\t" << "comp=" << comp);
	linphone_core_set_audio_jittcomp(mCore, comp);
}

int CoreAPI::getVideoJittcomp() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getVideoJittcomp", "this=" << this);
	return linphone_core_get_video_jittcomp(mCore);
}

void CoreAPI::setVideoJittcomp(int comp) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setVideoJittcomp", "this=" << this << "\t" << "comp=" << comp);
	linphone_core_set_video_jittcomp(mCore, comp);
}
	
int CoreAPI::getFirewallPolicy() const {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::getFirewallPolicy", "this=" << this);
	return linphone_core_get_firewall_policy(mCore);
}

void CoreAPI::setFirewallPolicy(int policy) {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::setFirewallPolicy", "this=" << this << "\t" << "policy=" << policy);
	linphone_core_set_firewall_policy(mCore, (LinphoneFirewallPolicy)policy);
}
	
int CoreAPI::getMediaEncryption() const {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::getMediaEncryption", "this=" << this);
	return linphone_core_get_media_encryption(mCore);
}

void CoreAPI::setMediaEncryption(int encryption) {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::setMediaEncryption", "this=" << this << "\t" << "encryption=" << encryption);
	linphone_core_set_media_encryption(mCore, (LinphoneMediaEncryption)encryption);
}
	

/*
 *
 * AuthInfo functions
 *
 */

void CoreAPI::addAuthInfo(const AuthInfoAPIPtr &authInfo) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::addAuthInfo", "this=" << this << "\t" << "authInfo=" << authInfo);
	linphone_core_add_auth_info(mCore, authInfo->getRef());
}

void CoreAPI::abortAuthentication(const AuthInfoAPIPtr &authInfo) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::abortAuthentication", "this=" << this << "\t" << "authInfo=" << authInfo);
	linphone_core_abort_authentication(mCore, authInfo->getRef());
}

void CoreAPI::removeAuthInfo(const AuthInfoAPIPtr &authInfo) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::removeAuthInfo", "this=" << this << "\t" << "authInfo=" << authInfo);
	linphone_core_remove_auth_info(mCore, authInfo->getRef());
}

AuthInfoAPIPtr CoreAPI::findAuthInfo(const StringPtr &realm, const StringPtr &username) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::findAuthInfo", "this=" << this << "\t" << "realm=" << realm << "\t" << "username=" << username);
	const LinphoneAuthInfo* authInfo = linphone_core_find_auth_info(mCore, STRING_TO_CHARPTR(realm), STRING_TO_CHARPTR(username));
	return getFactory()->getAuthInfo(authInfo);
}

std::vector<AuthInfoAPIPtr> CoreAPI::getAuthInfoList() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getAuthInfoList", "this=" << this);
	std::vector<AuthInfoAPIPtr> list;
	for (const MSList *node = linphone_core_get_auth_info_list(mCore); node != NULL; node = ms_list_next(node)) {
		list.push_back(getFactory()->getAuthInfo(reinterpret_cast<LinphoneAuthInfo*>(node->data)));
	}
	return list;
}

void CoreAPI::clearAllAuthInfo() {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::clearAllAuthInfo", "this=" << this);
	linphone_core_clear_all_auth_info(mCore);
}


/*
 *
 * Instantiator functions
 *
 */

FileManagerAPIPtr CoreAPI::getFileManager() const {
	
	FBLOG_DEBUG("CoreAPI::getFileManager", "this=" << this);
	if(!mFileManager) {
		mFileManager = getFactory()->getFileManager();
	}
	return mFileManager;
}

ProxyConfigAPIPtr CoreAPI::newProxyConfig() const {

	FBLOG_DEBUG("CoreAPI::newProxyConfig", "this=" << this);
	return getFactory()->getProxyConfig();
}

AuthInfoAPIPtr CoreAPI::newAuthInfo(const StringPtr &username, const StringPtr &userid, const StringPtr &passwd, const StringPtr &ha1,
		const StringPtr &realm) const {

	FBLOG_DEBUG("CoreAPI::newAuthInfo", "this=" << this
				<< "\t" << "username=" << username
				<< "\t" << "userid=" << userid
				<< "\t" << "passwd=" << passwd
				<< "\t" << "ha1=" << ha1
				<< "\t" << "realm=" << realm);
	return getFactory()->getAuthInfo(username, userid, passwd, ha1, realm);
}
	
AddressAPIPtr CoreAPI::newAddress(const StringPtr &address) const {
	
	FBLOG_DEBUG("CoreAPI::newAddress", "this=" << this << "\t" << "address=" << address);
	return getFactory()->getAddress(address);
}


/*
 *
 * DTMF functions
 *
 */

void CoreAPI::sendDtmf(const std::string &dtmf) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::sendDtmf", "this=" << this << "\t" << "dtmf=" << dtmf);
	if (dtmf.size() == 1)
		linphone_core_send_dtmf(mCore, dtmf[0]);
}

void CoreAPI::stopDtmf() {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::playDtmf", "this=" << this);
	linphone_core_stop_dtmf(mCore);
}

void CoreAPI::playDtmf(const std::string &dtmf, int duration_ms) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::playDtmf", "this=" << this << "\t" << "dtmf=" << dtmf << ", duration_ms=" << duration_ms);
	if (dtmf.size() == 1)
		linphone_core_play_dtmf(mCore, dtmf[0], duration_ms);
}

bool CoreAPI::getUseInfoForDtmf() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getUseInfoForDtmf", "this=" << this);
	return linphone_core_get_use_info_for_dtmf(mCore) == TRUE ? true : false;
}

void CoreAPI::setUseInfoForDtmf(bool enable) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setUseInfoForDtmf", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_set_use_info_for_dtmf(mCore, enable ? TRUE : FALSE);
}

bool CoreAPI::getUseRfc2833ForDtmf() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getUseRfc2833ForDtmf", "this=" << this);
	return linphone_core_get_use_rfc2833_for_dtmf(mCore) == TRUE ? true : false;

}

void CoreAPI::setUseRfc2833ForDtmf(bool enable) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setUseRfc2833ForDtmf", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_set_use_rfc2833_for_dtmf(mCore, enable ? TRUE : FALSE);
}

	
/*
 *
 * Presence functions
 *
 */
	
int CoreAPI::getPresenceInfo() const {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::getPresenceInfo", "this=" << this);
	return linphone_core_get_presence_info(mCore);
}

void CoreAPI::setPresenceInfo(int status) {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::setPresenceInfo", "this=" << this << "\t" << "status=" << status);
	FBLOG_WARN("CoreAPI::setPresenceInfo", "Not correcly wrapper !!!!");
	return linphone_core_set_presence_info(mCore, 0, NULL, (LinphoneOnlineStatus)status);
}

	
/*
 *
 * Misc functions
 *
 */

StringPtr CoreAPI::getVersion() const {
	FBLOG_DEBUG("CoreAPI::getVersion", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_version());
}

StringPtr CoreAPI::getPluginVersion() const {
	FBLOG_DEBUG("CoreAPI::getPluginVersion", "this=" << this);
	return CHARPTR_TO_STRING(FBSTRING_PLUGIN_VERSION);
}

void CoreAPI::enableEchoCancellation(bool enable) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableEchoCancellation", "this=" << this << "\t" << "enable="<< enable);

	linphone_core_enable_echo_cancellation(mCore, enable ? TRUE : FALSE);
}
bool CoreAPI::echoCancellationEnabled() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::echoCancellationEnabled", "this=" << this);
	return linphone_core_echo_cancellation_enabled(mCore) == TRUE ? true : false;
}

void CoreAPI::enableEchoLimiter(bool enable) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableEchoLimiter", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_enable_echo_limiter(mCore, enable ? TRUE : FALSE);
}

bool CoreAPI::echoLimiterEnabled() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::echoLimiterEnabled", "this=" << this);
	return linphone_core_echo_limiter_enabled(mCore) == TRUE ? true : false;
}

void CoreAPI::setStaticPictureFps(float fps) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setStaticPictureFps", "this=" << this << "\t" << "fps=" << fps);
	linphone_core_set_static_picture_fps(mCore, fps);
}

float CoreAPI::getStaticPictureFps() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getStaticPictureFps", "this=" << this);
	return linphone_core_get_static_picture_fps(mCore);
}

StringPtr CoreAPI::getUserAgentName() const {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::getUserAgentName", "this=" << this);
	FBLOG_ERROR("CoreAPI::getUserAgentName", "NOT IMPLEMENTED");
	//TODO
	//return CHARPTR_TO_STRING(linphone_core_get_user_agent_name(mCore));
	return StringPtr();
}

void CoreAPI::setUserAgentName(const StringPtr &name) {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::setUserAgentName", "this=" << this << "\t" << "name=" << name);
	FBLOG_ERROR("CoreAPI::setUserAgentName", "NOT IMPLEMENTED");
	//TODO
	//linphone_core_set_user_agent_name(mCore, name);
	return;
}

StringPtr CoreAPI::getUserAgentVersion() const {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::getUserAgentVersion", "this=" << this);
	FBLOG_ERROR("CoreAPI::getUserAgentVersion", "NOT IMPLEMENTED");
	//TODO
	//return CHARPTR_TO_STRING(linphone_core_get_user_agent_version(mCore));
	return StringPtr();
}

void CoreAPI::setUserAgentVersion(const StringPtr &version) {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::setUserAgentVersion", "this=" << this << "\t" << "version=" << version);
	FBLOG_ERROR("CoreAPI::setUserAgentVersion", "NOT IMPLEMENTED");
	//TODO
	//linphone_core_set_user_agent_version(mCore, name);
	return;
}
	
AddressAPIPtr CoreAPI::interpretUrl(const StringPtr &url) const {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::interpretUrl", "this=" << this << "\t" << "url=" << url);
	AddressAPIPtr address = getFactory()->getAddress(linphone_core_interpret_url(mCore, STRING_TO_CHARPTR(url)));
	address->own();
	return address;
}

/*
 *
 * File functions
 *
 */

IMPLEMENT_PROPERTY_FILE(CoreAPI, getRing, setRing);

StringPtr CoreAPI::getRing() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getRing", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_ring(mCore));
}

void CoreAPI::setRing(const StringPtr &ring) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setRing", "this=" << this << "\t" << "ring=" << ring);
	linphone_core_set_ring(mCore, STRING_TO_CHARPTR(ring));
}

IMPLEMENT_PROPERTY_FILE(CoreAPI, getRingback, setRingback);

StringPtr CoreAPI::getRingback() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getRingback", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_ringback(mCore));
}

void CoreAPI::setRingback(const StringPtr &ringback) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setRingback", "this=" << this << "\t" << "ringback=" << ringback);
	linphone_core_set_ringback(mCore, STRING_TO_CHARPTR(ringback));
}

IMPLEMENT_PROPERTY_FILE(CoreAPI, getPlayFile, setPlayFile);

StringPtr CoreAPI::getPlayFile() const {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::getPlayFile", "this=" << this);
	FBLOG_ERROR("CoreAPI::getPlayFile", "NOT IMPLEMENTED");
	//TODO STUB
	return StringPtr();// CHARPTR_TO_STRING(linphone_core_get_ringback(mCore));
}

void CoreAPI::setPlayFile(const StringPtr &playFile) {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::setPlayFile", "this=" << this << "\t" << "playFile=" << playFile);
	linphone_core_set_play_file(mCore, STRING_TO_CHARPTR(playFile));
}

IMPLEMENT_PROPERTY_FILE(CoreAPI, getRecordFile, setRecordFile);

StringPtr CoreAPI::getRecordFile() const {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::getRecordFile", "this=" << this);
	FBLOG_ERROR("CoreAPI::getRecordFile", "NOT IMPLEMENTED");
	//TODO STUB
	return StringPtr();// CHARPTR_TO_STRING(linphone_core_get_ringback(mCore));
}

void CoreAPI::setRecordFile(const StringPtr &recordFile) {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::setRecordFile", "this=" << this << "\t" << "recordFile=" << recordFile);
	linphone_core_set_record_file(mCore, STRING_TO_CHARPTR(recordFile));
}

IMPLEMENT_PROPERTY_FILE(CoreAPI, getRootCa, setRootCa);

StringPtr CoreAPI::getRootCa() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getRootCa", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_root_ca(mCore));
}

void CoreAPI::setRootCa(const StringPtr &rootCa) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setRootCa", "this=" << this << "\t" << "rootCa=" << rootCa);
	linphone_core_set_root_ca(mCore, STRING_TO_CHARPTR(rootCa));
}

IMPLEMENT_PROPERTY_FILE(CoreAPI, getStaticPicture, setStaticPicture);

StringPtr CoreAPI::getStaticPicture() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getStaticPicture", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_static_picture(mCore));
}

void CoreAPI::setStaticPicture(const StringPtr &staticPicture) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setStaticPicture", "this=" << this << "\t" << "staticPicture=" << staticPicture);
	linphone_core_set_static_picture(mCore, STRING_TO_CHARPTR(staticPicture));
}

IMPLEMENT_PROPERTY_FILE(CoreAPI, getZrtpSecretsFile, setZrtpSecretsFile);

StringPtr CoreAPI::getZrtpSecretsFile() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getZrtpSecretsFile", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_zrtp_secrets_file(mCore));
}

void CoreAPI::setZrtpSecretsFile(const StringPtr &secretsFile) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setZrtpSecretsFile", "this=" << this << "\t" << "secretsFile=" << secretsFile);
	linphone_core_set_zrtp_secrets_file(mCore, STRING_TO_CHARPTR(secretsFile));
}


///////////////////////////////////////////////////////////////////////////////
// Logs
///////////////////////////////////////////////////////////////////////////////

void CoreAPI::setLogHandler(const FB::JSObjectPtr &handler) {	
	FBLOG_DEBUG("CoreAPI::setLogHandler", "this=" << this << "\t" << "handler=" << handler);
	mLogHandler = handler;
}

FB::JSObjectPtr CoreAPI::getLogHandler() const {
	FBLOG_DEBUG("CoreAPI::getLogHandler", "this=" << this);
	return mLogHandler;
}


///////////////////////////////////////////////////////////////////////////////
// uPnP
///////////////////////////////////////////////////////////////////////////////

bool CoreAPI::upnpAvailable() const {
	FBLOG_DEBUG("CoreAPI::upnpAvailable", "this=" << this);
	return linphone_core_upnp_available() == TRUE ? true : false;
}

StringPtr CoreAPI::getUpnpExternalIpaddress() const {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::getUpnpExternalIpaddress", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_upnp_external_ipaddress(mCore));
}

LinphoneUpnpState CoreAPI::getUpnpState() const {
	FB_ASSERT_CORE
	CORE_MUTEX
	
	FBLOG_DEBUG("CoreAPI::getUpnpState", "this=" << this);
	return linphone_core_get_upnp_state(mCore);
}


///////////////////////////////////////////////////////////////////////////////
// Tunnel
///////////////////////////////////////////////////////////////////////////////

bool CoreAPI::tunnelAvailable() const {
	FBLOG_DEBUG("CoreAPI::tunnelAvailable", "this=" << this);
	return linphone_core_tunnel_available() == TRUE ? true : false;
}


///////////////////////////////////////////////////////////////////////////////
// Callbacks
///////////////////////////////////////////////////////////////////////////////

void CoreAPI::onGlobalStateChanged(LinphoneGlobalState gstate, const char *message) {
	FBLOG_DEBUG("CoreAPI::onGlobalStateChanged",  "this=" << this << "\t" << "gstate=" << gstate << "\t" << "message=" << message);
	fire_globalStateChanged(boost::static_pointer_cast<CoreAPI>(shared_from_this()), gstate, CHARPTR_TO_STRING(message));
}

void CoreAPI::onRegistrationStateChanged(LinphoneProxyConfig *cfg, LinphoneRegistrationState rstate, const char *message) {
	FBLOG_DEBUG("CoreAPI::onRegistrationStateChanged",  "this=" << this << "\t" << "cfg=" << cfg << "\t" << "rstate=" << rstate << "\t" << "message=" << message);
	fire_registrationStateChanged(boost::static_pointer_cast<CoreAPI>(shared_from_this()), getFactory()->getProxyConfig(cfg), rstate, CHARPTR_TO_STRING(message));
}

void CoreAPI::onCallStateChanged(LinphoneCall *call, LinphoneCallState cstate, const char *message) {
	FBLOG_DEBUG("CoreAPI::onCallStateChanged",  "this=" << this << "\t" << "call=" << call << "\t" << "cstate=" << cstate << "\t" << "message=" << message);
	fire_callStateChanged(boost::static_pointer_cast<CoreAPI>(shared_from_this()), getFactory()->getCall(call), cstate, CHARPTR_TO_STRING(message));
}

void CoreAPI::onNotifyPresenceRecv(LinphoneFriend * lf) {
	FBLOG_DEBUG("CoreAPI::onNotifyPresenceRecv",  "this=" << this << "\t" << "lf=" << lf);
}

void CoreAPI::onNewSubscriptionRequest(LinphoneFriend *lf, const char *url) {
	FBLOG_DEBUG("CoreAPI::onNewSubscriptionRequest",  "this=" << this << "\t" << "lf=" << lf << "\t" << "url=" << url);
}

void CoreAPI::onAuthInfoRequested(const char *realm, const char *username) {
	FBLOG_DEBUG("CoreAPI::onAuthInfoRequested",  "this=" << this << "\t" << "realm=" << realm << "\t" << "username=" << username);
	fire_authInfoRequested(boost::static_pointer_cast<CoreAPI>(shared_from_this()), CHARPTR_TO_STRING(realm), CHARPTR_TO_STRING(username));
}

void CoreAPI::onCallLogUpdated(LinphoneCallLog *newcl) {
	FBLOG_DEBUG("CoreAPI::onCallLogUpdated",  "this=" << this << "\t" << "newcl=" << newcl);
}

void CoreAPI::onTextReceived(LinphoneChatRoom *room, const LinphoneAddress *from, const char *message) {
	FBLOG_DEBUG("CoreAPI::onTextReceived",  "this=" << this << "\t" << "room=" << room << "\t" << "from=" << from << "message=" << message);
}

void CoreAPI::onDtmfReceived(LinphoneCall *call, int dtmf) {
	FBLOG_DEBUG("CoreAPI::onDtmfReceived",  "this=" << this << "\t" << "call=" << call << "\t" << "dtmf=" << dtmf);
}

void CoreAPI::onReferReceived(const char *refer_to) {
	FBLOG_DEBUG("CoreAPI::onReferReceived",  "this=" << this << "\t" << "refer_to=" << refer_to);
}

void CoreAPI::onBuddyInfoUpdated(LinphoneFriend *lf) {
	FBLOG_DEBUG("CoreAPI::onBuddyInfo_updated",  "this=" << this << "\t" << "lf=" << lf);
}

void CoreAPI::onNotifyRecv(LinphoneCall *call, const char *from, const char *event) {
	FBLOG_DEBUG("CoreAPI::onNotifyRecv",  "this=" << this << "\t" << "call=" << call  << "\t" << "from=" << from << "\t" << "event=" << event);
}

void CoreAPI::onDisplayStatus(const char *message) {
	FBLOG_DEBUG("CoreAPI::onDisplayStatus",  "this=" << this << "\t" << "message=" << message);
	fire_displayStatus(boost::static_pointer_cast<CoreAPI>(shared_from_this()), CHARPTR_TO_STRING(message));
}

void CoreAPI::onDisplayMessage(const char *message) {
	FBLOG_DEBUG("CoreAPI::onDisplayMessage",  "this=" << this << "\t" << "message=" << message);
	fire_displayMessage(boost::static_pointer_cast<CoreAPI>(shared_from_this()), CHARPTR_TO_STRING(message));
}

void CoreAPI::onDisplayWarning(const char *message) {
	FBLOG_DEBUG("CoreAPI::onDisplayWarning",  "this=" << this << "\t" << "message=" << message);
	fire_displayWarning(boost::static_pointer_cast<CoreAPI>(shared_from_this()), CHARPTR_TO_STRING(message));
}

void CoreAPI::onDisplayUrl(const char *message, const char *url) {
	FBLOG_DEBUG("CoreAPI::onDisplayUrl",  "this=" << this << "\t" << "message=" << message << "\t" << "url=" << url);
	fire_displayUrl(boost::static_pointer_cast<CoreAPI>(shared_from_this()), CHARPTR_TO_STRING(message), CHARPTR_TO_STRING(url));
}

void CoreAPI::onShow() {
	FBLOG_DEBUG("CoreAPI::onShow",  "this=" << this);
	fire_show(boost::static_pointer_cast<CoreAPI>(shared_from_this()));
}

void CoreAPI::onCallEncryptionChanged(LinphoneCall *call, bool_t on, const char *authentication_token) {
	FBLOG_DEBUG("CoreAPI::onCallEncryptionChanged",  "this=" << this << "\t" << "call=" << call << "\t" << "on=" << on << "\t" << "authentication_token=" << authentication_token);
}

///////////////////////////////////////////////////////////////////////////////
// Wrappers
///////////////////////////////////////////////////////////////////////////////

// Global callbacks which wraps CoreAPI object methods
#define GLC_DEFINED() (linphone_core_get_user_data(lc) != NULL)
#define GLC_THIS() ((CoreAPI *)linphone_core_get_user_data(lc))

void CoreAPI::wrapper_global_state_changed(LinphoneCore *lc, LinphoneGlobalState gstate, const char *message) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onGlobalStateChanged(gstate, message);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_global_state_changed", "No proxy defined !");
	}
}
void CoreAPI::wrapper_registration_state_changed(LinphoneCore *lc, LinphoneProxyConfig *cfg, LinphoneRegistrationState cstate, const char *message) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onRegistrationStateChanged(cfg, cstate, message);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_registration_state_changed", "No proxy defined !");
	}
}
void CoreAPI::wrapper_call_state_changed(LinphoneCore *lc, LinphoneCall *call, LinphoneCallState cstate, const char *message) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onCallStateChanged(call, cstate, message);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_call_state_changed", "No proxy defined !");
	}
}
void CoreAPI::wrapper_notify_presence_recv(LinphoneCore *lc, LinphoneFriend * lf) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onNotifyPresenceRecv(lf);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_notify_presence_recv", "No proxy defined !");
	}
}
void CoreAPI::wrapper_new_subscription_request(LinphoneCore *lc, LinphoneFriend *lf, const char *url) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onNewSubscriptionRequest(lf, url);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_new_subscription_request", "No proxy defined !");
	}
}
void CoreAPI::wrapper_auth_info_requested(LinphoneCore *lc, const char *realm, const char *username) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onAuthInfoRequested(realm, username);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_auth_info_requested", "No proxy defined !");
	}
}
void CoreAPI::wrapper_call_log_updated(LinphoneCore *lc, LinphoneCallLog *newcl) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onCallLogUpdated(newcl);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_call_log_updated", "No proxy defined !");
	}
}
void CoreAPI::wrapper_text_received(LinphoneCore *lc, LinphoneChatRoom *room, const LinphoneAddress *from, const char *message) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onTextReceived(room, from, message);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_text_received", "No proxy defined !");
	}
}
void CoreAPI::wrapper_dtmf_received(LinphoneCore *lc, LinphoneCall *call, int dtmf) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onDtmfReceived(call, dtmf);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_dtmf_received", "No proxy defined !");
	}
}
void CoreAPI::wrapper_refer_received(LinphoneCore *lc, const char *refer_to) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onReferReceived(refer_to);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_refer_received", "No proxy defined !");
	}
}
void CoreAPI::wrapper_buddy_info_updated(LinphoneCore *lc, LinphoneFriend *lf) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onBuddyInfoUpdated(lf);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_buddy_info_updated", "No proxy defined !");
	}
}
void CoreAPI::wrapper_notify_recv(LinphoneCore *lc, LinphoneCall *call, const char *from, const char *event) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onNotifyRecv(call, from, event);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_notify_recv", "No proxy defined !");
	}
}
void CoreAPI::wrapper_display_status(LinphoneCore *lc, const char *message) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onDisplayStatus(message);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_display_status", "No proxy defined !");
	}
}
void CoreAPI::wrapper_display_message(LinphoneCore *lc, const char *message) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onDisplayMessage(message);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_display_message", "No proxy defined !");
	}
}
void CoreAPI::wrapper_display_warning(LinphoneCore *lc, const char *message) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onDisplayWarning(message);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_display_warning", "No proxy defined !");
	}
}
void CoreAPI::wrapper_display_url(LinphoneCore *lc, const char *message, const char *url) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onDisplayUrl(message, url);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_display_url", "No proxy defined !");
	}
}
void CoreAPI::wrapper_show(LinphoneCore *lc) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onShow();
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_show", "No proxy defined !");
	}
}
void CoreAPI::wrapper_call_encryption_changed(LinphoneCore *lc, LinphoneCall *call, bool_t on, const char *authentication_token) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onCallEncryptionChanged(call, on, authentication_token);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_call_encryption_changed", "No proxy defined !");
	}
}

} // LinphoneWeb
