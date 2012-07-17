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
#include <fstream>
#include "coreapi.h"

#ifndef WIN32
#else
#include <windows.h>
void usleep(int waitTime) {
	Sleep(waitTime/1000);
}
#endif

#define CORE_MUTEX boost::mutex::scoped_lock scopedLock(m_core_mutex);

static boost::mutex sInstanceMutex;
static int sInstanceCount = 0;

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

	threads->interrupt_all();
	threads->join_all();

	if (core != NULL) {
		linphone_core_destroy(core);
	}

	sInstanceMutex.lock();
	--sInstanceCount;
	sInstanceMutex.unlock();
	FBLOG_DEBUG("linphone_destroy_thread", "end");
}

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
		JSAPIAuto(APIDescription(this)), m_plugin(plugin), m_host(host), mCore(NULL), m_core_thread(NULL), m_threads(new mythread_group()) {
	FBLOG_DEBUG("CoreAPI::CoreAPI()", "this=" << this);
	initProxy();
}

void CoreAPI::initProxy() {
	// Read-only property
	registerProperty("version", make_property(this, &CoreAPI::getVersion));
	registerProperty("pluginVersion", make_property(this, &CoreAPI::getPluginVersion));
	registerProperty("sip_port", make_property(this, &CoreAPI::getSipPort));

	// Propery
	registerProperty("magic", make_property(this, &CoreAPI::getMagic, &CoreAPI::setMagic));

	registerMethod("init", make_method(this, &CoreAPI::init));

	// Call bindings
	REGISTER_SYNC_N_ASYNC("invite", invite);
	registerMethod("acceptCall", make_method(this, &CoreAPI::acceptCall));
	registerMethod("terminateCall", make_method(this, &CoreAPI::terminateCall));

	// Levels bindings
	registerProperty("playLevel", make_property(this, &CoreAPI::getPlayLevel, &CoreAPI::setPlayLevel));
	registerProperty("recLevel", make_property(this, &CoreAPI::getRecLevel, &CoreAPI::setRecLevel));
	registerProperty("ringLevel", make_property(this, &CoreAPI::getRingLevel, &CoreAPI::setRingLevel));

	// Video bindings
	registerMethod("videoSupported", make_method(this, &CoreAPI::videoSupported));
	registerProperty("videoPreviewEnabled", make_property(this, &CoreAPI::videoPreviewEnabled, &CoreAPI::enableVideoPreview));
	registerProperty("videoEnabled", make_property(this, &CoreAPI::videoEnabled, &CoreAPI::enableVideo));
	registerProperty("nativePreviewWindowId", make_property(this, &CoreAPI::getNativePreviewWindowId, &CoreAPI::setNativePreviewWindowId));
	registerProperty("nativeVideoWindowId", make_property(this, &CoreAPI::getNativeVideoWindowId, &CoreAPI::setNativeVideoWindowId));

	// Sound device bindings
	registerMethod("reloadSoundDevices", make_method(this, &CoreAPI::reloadSoundDevices));
	registerMethod("getSoundDevices", make_method(this, &CoreAPI::getSoundDevices));
	registerMethod("soundDeviceCanCapture", make_method(this, &CoreAPI::soundDeviceCanCapture));
	registerMethod("soundDeviceCanPlayback", make_method(this, &CoreAPI::soundDeviceCanPlayback));
	registerProperty("ringerDevice", make_property(this, &CoreAPI::getRingerDevice, &CoreAPI::setRingerDevice));
	registerProperty("captureDevice", make_property(this, &CoreAPI::getCaptureDevice, &CoreAPI::setCaptureDevice));
	registerProperty("playbackDevice", make_property(this, &CoreAPI::getPlaybackDevice, &CoreAPI::setPlaybackDevice));

	// Video device bindings
	registerMethod("reloadVideoDevices", make_method(this, &CoreAPI::reloadVideoDevices));
	registerMethod("getVideoDevices", make_method(this, &CoreAPI::getVideoDevices));
	registerProperty("videoDevice", make_property(this, &CoreAPI::getVideoDevice, &CoreAPI::setVideoDevice));

	// Codecs bindings
	registerMethod("getAudioCodecs", make_method(this, &CoreAPI::getAudioCodecs));
	registerMethod("getVideoCodecs", make_method(this, &CoreAPI::getVideoCodecs));
	registerMethod("setAudioCodecs", make_method(this, &CoreAPI::setAudioCodecs));
	registerMethod("setVideoCodecs", make_method(this, &CoreAPI::setVideoCodecs));

	// ProxyConfig bindings
	registerMethod("addProxyConfig", make_method(this, &CoreAPI::addProxyConfig));
	registerMethod("clearProxyConfig", make_method(this, &CoreAPI::clearProxyConfig));
	registerMethod("removeProxyConfig", make_method(this, &CoreAPI::removeProxyConfig));
	registerMethod("getProxyConfigList", make_method(this, &CoreAPI::getProxyConfigList));
	registerMethod("setDefaultProxy", make_method(this, &CoreAPI::setDefaultProxy));
	registerMethod("getDefaultProxy", make_method(this, &CoreAPI::getDefaultProxy));

	// AuthInfo bindings
	registerMethod("addAuthInfo", make_method(this, &CoreAPI::addAuthInfo));

	// Initiator bindings
	registerMethod("newProxyConfig", make_method(this, &CoreAPI::newProxyConfig));
	registerMethod("newAuthInfo", make_method(this, &CoreAPI::newAuthInfo));

	// Dtmf
	registerMethod("sendDtmf", make_method(this, &CoreAPI::sendDtmf));
	registerMethod("playDtmf", make_method(this, &CoreAPI::playDtmf));
	registerMethod("stopDtmf", make_method(this, &CoreAPI::stopDtmf));

	// Miscs
	registerProperty("echoCancellationEnabled", make_property(this, &CoreAPI::echoCancellationEnabled, &CoreAPI::enableEchoCancellation));
	registerProperty("echoLimiterEnabled", make_property(this, &CoreAPI::echoLimiterEnabled, &CoreAPI::enableEchoLimiter));
	registerProperty("ipv6Enabled", make_property(this, &CoreAPI::ipv6Enabled, &CoreAPI::enableIpv6));
	registerProperty("keepAliveEnabled", make_property(this, &CoreAPI::keepAliveEnabled, &CoreAPI::enableKeepAlive));

	registerProperty("ring", make_property(this, &CoreAPI::getRing, &CoreAPI::setRing));
	registerMethod("setRingAsync", make_method(this, &CoreAPI::setRingAsync));
}

int CoreAPI::init() {
	FBLOG_DEBUG("CoreAPI::init()", "this=" << this);
	boost::mutex::scoped_lock scoped_instance_count_lock(sInstanceMutex);

	if (sInstanceCount == 0) {
		++sInstanceCount;
		srand((unsigned int) time(NULL));

#ifndef NDEBUG
		// Disable logs
		linphone_core_disable_logs();
#endif
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

		FBLOG_DEBUG("linphone_core_set_ring", "linphone_core_set_ring" << linphone_core_get_ring(mCore));

		m_core_thread = new boost::thread(linphone_iterate_thread, this);
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

		boost::thread t(linphone_destroy_thread, mCore, m_core_thread, m_threads);
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

const std::string &CoreAPI::getMagic() {
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

CallAPIPtr CoreAPI::invite(const std::string &dest) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::invite", "this=" << this << "\t" << "dest=" << dest);
	LinphoneCall *call = linphone_core_invite(mCore, dest.c_str());
	CallAPIPtr shared_call = CallAPI::get(call);
	return shared_call;
}

void CoreAPI::terminateCall(const CallAPIPtr &call) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::terminateCall", "this=" << this << "\t" << "call=" << call);
	linphone_core_terminate_call(mCore, call->getRef());
}

void CoreAPI::acceptCall(const CallAPIPtr &call) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::acceptCall", "this=" << this << "\t" << "call=" << call);
	linphone_core_accept_call(mCore, call->getRef());
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

int CoreAPI::getPlayLevel() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getPlayLevel", "this=" << this);
	return linphone_core_get_play_level(mCore);
}

void CoreAPI::setRecLevel(int level) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setRecLevel", "this=" << this << "\t" << "level=" << level);
	linphone_core_set_rec_level(mCore, level);
}

int CoreAPI::getRecLevel() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getRecLevel", "this=" << this);
	return linphone_core_get_rec_level(mCore);
}

void CoreAPI::setRingLevel(int level) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setRingLevel", "this=" << this << "\t" << "level=" << level);
	linphone_core_set_ring_level(mCore, level);
}

int CoreAPI::getRingLevel() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getRingLevel", "this=" << this);
	return linphone_core_get_ring_level(mCore);
}

/*
 *
 * Video functions
 *
 */

bool CoreAPI::videoSupported() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::videoSupported()", "this=" << this);
	return linphone_core_video_supported(mCore) == TRUE ? true : false;
}
void CoreAPI::enableVideo(bool enable) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableVideo()", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_enable_video(mCore, enable ? TRUE : FALSE, enable ? TRUE : FALSE);
}

bool CoreAPI::videoEnabled() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::videoEnabled()", "this=" << this);
	return linphone_core_video_enabled(mCore) == TRUE ? true : false;
}

void CoreAPI::enableVideoPreview(bool enable) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableVideoPreview()", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_enable_video_preview(mCore, enable ? TRUE : FALSE);
}

bool CoreAPI::videoPreviewEnabled() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::videoPreviewEnabled()", "this=" << this);
	return linphone_core_video_preview_enabled(mCore) == TRUE ? true : false;
}

void CoreAPI::setNativeVideoWindowId(unsigned long id) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setNativeVideoWindowId()", "this=" << this << "\t" << "id=" << id);
	linphone_core_set_native_video_window_id(mCore, id);
}

unsigned long CoreAPI::getNativeVideoWindowId() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getNativeVideoWindowId()", "this=" << this);
	return linphone_core_get_native_video_window_id(mCore);
}

void CoreAPI::setNativePreviewWindowId(unsigned long id) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setNativePreviewWindowId()", "this=" << this << "\t" << "id=" << id);
	linphone_core_set_native_preview_window_id(mCore, id);
}

unsigned long CoreAPI::getNativePreviewWindowId() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getNativePreviewWindowId()", "this=" << this);
	return linphone_core_get_native_preview_window_id(mCore);
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

FB::VariantList CoreAPI::getSoundDevices() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getSoundDevices()", "this=" << this);
	FB::VariantList list;
	const char **devlist = linphone_core_get_sound_devices(mCore);
	while (devlist != NULL && *devlist != NULL) {
		list.push_back(std::string(*devlist++));
	}

	return list;
}

bool CoreAPI::soundDeviceCanCapture(const std::string &devid) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::soundDeviceCanCapture()", "this=" << this << "\t" << "devid=" << devid);
	return linphone_core_sound_device_can_capture(mCore, devid.c_str()) == TRUE ? true : false;
}

bool CoreAPI::soundDeviceCanPlayback(const std::string &devid) {
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

std::string CoreAPI::getRingerDevice() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getRingerDevice()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_ringer_device(mCore));
}

std::string CoreAPI::getPlaybackDevice() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getPlaybackDevice()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_playback_device(mCore));
}

std::string CoreAPI::getCaptureDevice() {
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

FB::VariantList CoreAPI::getVideoDevices() {
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

std::string CoreAPI::getVideoDevice() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getVideoDevice()", "this=" << this);

	return CHARPTR_TO_STRING(linphone_core_get_video_device(mCore));
}

/*
 *
 * Codecs functions
 *
 */

FB::VariantList CoreAPI::getAudioCodecs() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getAudioCodecs()", "this=" << this);
	FB::VariantList list;
	for (const MSList *node = linphone_core_get_audio_codecs(mCore); node != NULL; node = ms_list_next(node)) {
		list.push_back(PayloadTypeAPI::get(boost::static_pointer_cast<CoreAPI>(shared_from_this()), reinterpret_cast<PayloadType*>(node->data)));
	}
	return list;
}

FB::VariantList CoreAPI::getVideoCodecs() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getVideoCodecs()", "this=" << this);
	FB::VariantList list;
	for (const MSList *node = linphone_core_get_video_codecs(mCore); node != NULL; node = ms_list_next(node)) {
		list.push_back(PayloadTypeAPI::get(boost::static_pointer_cast<CoreAPI>(shared_from_this()), reinterpret_cast<PayloadType*>(node->data)));
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
FB::VariantList CoreAPI::getProxyConfigList() {
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
ProxyConfigAPIPtr CoreAPI::getDefaultProxy() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getDefaultProxy()", "this=" << this);
	LinphoneProxyConfig *ptr = NULL;
	linphone_core_get_default_proxy(mCore, &ptr);
	if (ptr != NULL)
		return ProxyConfigAPI::get(ptr);
	return ProxyConfigAPIPtr();
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

/*
 *
 * Misc functions
 *
 */

std::string CoreAPI::getVersion() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getVersion()", "this=" << this);
	return linphone_core_get_version();
}

std::string CoreAPI::getPluginVersion() {
	FBLOG_DEBUG("CoreAPI::getPluginVersion()", "this=" << this << FBSTRING_PLUGIN_VERSION);
	return FBSTRING_PLUGIN_VERSION;
}

int CoreAPI::getSipPort() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getSipPort()", "this=" << this);
	return linphone_core_get_sip_port(mCore);
}

void CoreAPI::enableEchoCancellation(bool enable) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableEchoCancellation()", "this=" << this << "\t" << "enable="<< enable);

	linphone_core_enable_echo_cancellation(mCore, enable ? TRUE : FALSE);
}
bool CoreAPI::echoCancellationEnabled() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::echoCancellationEnabled()", "this=" << this);
	return linphone_core_echo_cancellation_enabled(mCore) == TRUE ? true : false;
}

void CoreAPI::enableEchoLimiter(bool enable) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableEchoLimiter()", "this=" << this << "\t" << "enable="<< enable);
	linphone_core_enable_echo_limiter(mCore, enable ? TRUE : FALSE);
}
bool CoreAPI::echoLimiterEnabled() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::echoLimiterEnabled()", "this=" << this);
	return linphone_core_echo_limiter_enabled(mCore) == TRUE ? true : false;
}

void CoreAPI::enableIpv6(bool enable) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableIpv6()", "this=" << this << "\t" << "enable="<< enable);
	linphone_core_enable_ipv6(mCore, enable ? TRUE : FALSE);
}
bool CoreAPI::ipv6Enabled() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::ipv6Enabled()", "this=" << this);
	return linphone_core_ipv6_enabled(mCore) == TRUE ? true : false;
}

void CoreAPI::enableKeepAlive(bool enable) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableKeepAlive()", "this=" << this << "\t" << "enable="<< enable);
	linphone_core_enable_keep_alive(mCore, enable ? TRUE : FALSE);
}
bool CoreAPI::keepAliveEnabled() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::keepAliveEnabled()", "this=" << this);
	return linphone_core_keep_alive_enabled(mCore) == TRUE ? true : false;
}

/*
 *
 * File functions
 *
 */

std::string CoreAPI::getRing() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getRing()", "this=" << this);
	return m_internal_ring;
}

void CoreAPI::setRing(const std::string &ring) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setRing()", "this=" << this << "\t" << "ring=" << ring);
	download(ring, boost::bind(&CoreAPI::setRingCallback, this, _1, _2, FB::JSObjectPtr()));
}

void CoreAPI::setRingAsync(const std::string &ring, const FB::JSObjectPtr& callback) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setRingAsync()", "this=" << this<< "\t" << "ring=" << ring);
	download(ring, boost::bind(&CoreAPI::setRingCallback, this, _1, _2, callback));
}

void CoreAPI::setRingCallback(const std::string& url, const std::string& file, const FB::JSObjectPtr& callback) {
	FBLOG_DEBUG("CoreAPI::setRingCallback()", "this=" << this<< "\t" << "url=" << url << ", file=" << file);

	bool result = false;
	if(!file.empty()){
		CORE_MUTEX

		linphone_core_set_ring(mCore, file.c_str());
		m_internal_ring = url;
		result = true;
	}

	// Set
	if (callback) {
		callback->InvokeAsync("", FB::variant_list_of(shared_from_this())(result));
	}
}

/*
 *
 * Download functions
 *
 */

void CoreAPI::download(const std::string& url, const DownloadCallbackType& functionback) {
	FBLOG_DEBUG("CoreAPI::download()", "this=" << this << "\t" << "url="<< url);
	FB::SimpleStreamHelper::AsyncGet(m_host, FB::URI::fromString(url), boost::bind(&CoreAPI::downloadCallback, this, url, _1, _2, _3, _4, functionback));
}

void CoreAPI::downloadCallback(const std::string& url, bool success, const FB::HeaderMap& headers, const boost::shared_array<uint8_t>& data,
		const size_t size, const DownloadCallbackType &functionback) {
	static const char *prefix = "/linphone-web-";
	if (success) {
		FBLOG_DEBUG("CoreAPI::downloadCallback()", "Download " << url << " success");

		// Build path
		std::stringstream ss;
		ss << FB::System::getTempPath();
		if (ss.str().empty()) {
			FBLOG_ERROR("CoreAPI::downloadCallback", "Invalid temp dir");
			functionback(url, "");
			return;
		}
		ss << prefix << rand();
		FBLOG_DEBUG("CoreAPI::downloadCallback()", "Write to " << ss.str());

		// Write to file
		std::ofstream downloadedFile(ss.str().c_str(), std::ios_base::binary | std::ios_base::out);
		downloadedFile.write(reinterpret_cast<const char*>(data.get()), size);
		if (downloadedFile.bad()) {
			downloadedFile.close();
			FBLOG_DEBUG("CoreAPI::downloadCallback()", "Can't write into file " << ss);
			functionback(url, "");
			return;
		}
		downloadedFile.close();
		functionback(url, ss.str());
	} else {
		FBLOG_WARN("CoreAPI::downloadCallback()", "Download " << url << " failure");
		functionback(url, "");
	}
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
