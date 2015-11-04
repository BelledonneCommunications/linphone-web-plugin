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
 - Ghislain MARY <ghislain.mary@belledonne-communications.com>

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
#include "chatmessageapi.h"
#include "chatroomapi.h"
#include "coreapi.h"
#include "friendapi.h"
#include "msvideosizeapi.h"
#include "payloadtypeapi.h"
#include "proxyconfigapi.h"
#include "siptransportsapi.h"
#include "videopolicyapi.h"

#include <mediastreamer2/mscommon.h>

namespace LinphoneWeb {

#ifndef WIN32
#else //WIN32
#include <windows.h>
void usleep(int waitTime) {
	Sleep(waitTime/1000);
}
#endif //WIN32


void CoreAPI::iterateWithMutex() {
	CORE_MUTEX

	iterate();
}

///////////////////////////////////////////////////////////////////////////////
/// @fn CoreAPI::CoreAPI(corePtr const &plugin, const FB::BrowserHostPtr host)
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
	mTimer = FB::Timer::getTimer(mIterateInterval, true, boost::bind(&CoreAPI::iterateWithMutex, this));
}

void CoreAPI::initProxy() {
	// Read-only property
	registerProperty("version", make_property(this, &CoreAPI::getVersion));
	registerProperty("pluginVersion", make_property(this, &CoreAPI::getPluginVersion));
	registerProperty("depsVersion", make_property(this, &CoreAPI::getDepsVersion));
	registerProperty("rootfsVersion", make_property(this, &CoreAPI::getRootfsVersion));

	// Property
	registerProperty("magic", make_property(this, &CoreAPI::getMagic, &CoreAPI::setMagic));

	// Core helpers
	registerMethod("init", make_method(this, &CoreAPI::init));
	registerMethod("initFromConfig", make_method(this, &CoreAPI::initFromConfig));
	registerMethod("uninit", make_method(this, &CoreAPI::uninit));
	registerProperty("iterateEnabled", make_property(this, &CoreAPI::iterateEnabled, &CoreAPI::enableIterate));
	registerProperty("iterateInterval", make_property(this, &CoreAPI::getIterateInterval, &CoreAPI::setIterateInterval));
	registerProperty("config", make_property(this, &CoreAPI::getConfig));

	// Call bindings
	registerMethod("invite", make_method(this, &CoreAPI::invite));
	registerMethod("inviteAddress", make_method(this, &CoreAPI::inviteAddress));
	registerMethod("inviteWithParams", make_method(this, &CoreAPI::inviteWithParams));
	registerMethod("inviteAddressWithParams", make_method(this, &CoreAPI::inviteAddressWithParams));
	registerMethod("acceptCall", make_method(this, &CoreAPI::acceptCall));
	registerMethod("acceptCallWithParams", make_method(this, &CoreAPI::acceptCallWithParams));
	registerMethod("acceptEarlyMedia", make_method(this, &CoreAPI::acceptEarlyMedia));
	registerMethod("acceptEarlyMediaWithParams", make_method(this, &CoreAPI::acceptEarlyMediaWithParams));
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
	registerMethod("createCallParams", make_method(this, &CoreAPI::createCallParams));
	registerProperty("incTimeout", make_property(this, &CoreAPI::getIncTimeout, &CoreAPI::setIncTimeout));
	registerProperty("inCallTimeout", make_property(this, &CoreAPI::getInCallTimeout, &CoreAPI::setInCallTimeout));
	registerProperty("maxCalls", make_property(this, &CoreAPI::getMaxCalls, &CoreAPI::setMaxCalls));
	registerProperty("calls", make_property(this, &CoreAPI::getCalls));
	registerMethod("findCallFromUri", make_method(this, &CoreAPI::findCallFromUri));
	registerProperty("callsNb", make_property(this, &CoreAPI::getCallsNb));
	registerProperty("missedCallsCount", make_property(this, &CoreAPI::getMissedCallsCount));
	registerMethod("resetMissedCallsCount", make_method(this, &CoreAPI::resetMissedCallsCount));
	registerProperty("ringDuringIncomingEarlyMedia", make_property(this, &CoreAPI::getRingDuringIncomingEarlyMedia, &CoreAPI::setRingDuringIncomingEarlyMedia));

	// Conference bindings
	registerMethod("addAllToConference", make_method(this, &CoreAPI::addAllToConference));
	registerMethod("addToConference", make_method(this, &CoreAPI::addToConference));
	registerMethod("enterConference", make_method(this, &CoreAPI::enterConference));
	registerProperty("conferenceSize", make_property(this, &CoreAPI::getConferenceSize));
	registerMethod("inConference", make_method(this, &CoreAPI::isInConference));
	registerMethod("leaveConference", make_method(this, &CoreAPI::leaveConference));
	registerMethod("removeFromConference", make_method(this, &CoreAPI::removeFromConference));
	registerMethod("terminateConference", make_method(this, &CoreAPI::terminateConference));

	// Chat bindings
	REGISTER_PROPERTY_FILE(CoreAPI, "chatDatabasePath", getChatDatabasePath, setChatDatabasePath);
	registerProperty("chatEnabled", make_property(this, &CoreAPI::chatEnabled));
	registerProperty("chatRooms", make_property(this, &CoreAPI::getChatRooms));
	registerProperty("fileTransferServer", make_property(this, &CoreAPI::getFileTransferServer, &CoreAPI::setFileTransferServer));
	registerMethod("disableChat", make_method(this, &CoreAPI::disableChat));
	registerMethod("enableChat", make_method(this, &CoreAPI::enableChat));
	registerMethod("getChatRoom", make_method(this, &CoreAPI::getChatRoom));
	registerMethod("getChatRoomFromUri", make_method(this, &CoreAPI::getChatRoomFromUri));

	// Levels bindings
	registerProperty("playLevel", make_property(this, &CoreAPI::getPlayLevel, &CoreAPI::setPlayLevel));
	registerProperty("recLevel", make_property(this, &CoreAPI::getRecLevel, &CoreAPI::setRecLevel));
	registerProperty("ringLevel", make_property(this, &CoreAPI::getRingLevel, &CoreAPI::setRingLevel));
	registerProperty("micEnabled", make_property(this, &CoreAPI::micEnabled, &CoreAPI::enableMic));
	registerProperty("micGainDb", make_property(this, &CoreAPI::getMicGainDb, &CoreAPI::setMicGainDb));
	registerProperty("playbackGainDb", make_property(this, &CoreAPI::getPlaybackGainDb, &CoreAPI::setPlaybackGainDb));

	// Video bindings
	registerMethod("videoSupported", make_method(this, &CoreAPI::videoSupported));
	registerProperty("videoPreviewEnabled", make_property(this, &CoreAPI::videoPreviewEnabled, &CoreAPI::enableVideoPreview));
	registerProperty("selfViewEnabled", make_property(this, &CoreAPI::selfViewEnabled, &CoreAPI::enableSelfView));
	registerProperty("videoCaptureEnabled", make_property(this, &CoreAPI::videoCaptureEnabled, &CoreAPI::enableVideoCapture));
	registerProperty("videoDisplayEnabled", make_property(this, &CoreAPI::videoDisplayEnabled, &CoreAPI::enableVideoDisplay));
	registerProperty("nativePreviewWindowId", make_property(this, &CoreAPI::getNativePreviewWindowId, &CoreAPI::setNativePreviewWindowId));
	registerProperty("nativeVideoWindowId", make_property(this, &CoreAPI::getNativeVideoWindowId, &CoreAPI::setNativeVideoWindowId));
	registerProperty("usePreviewWindow", make_property(this, &CoreAPI::getUsePreviewWindow, &CoreAPI::setUsePreviewWindow));
	registerProperty("videoPolicy", make_property(this, &CoreAPI::getVideoPolicy, &CoreAPI::setVideoPolicy));
	registerProperty("preferredVideoSize", make_property(this, &CoreAPI::getPreferredVideoSize, &CoreAPI::setPreferredVideoSize));
	registerProperty("preferredVideoSizeByName", make_property(this, &CoreAPI::getPreferredVideoSizeByName, &CoreAPI::setPreferredVideoSizeByName));

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
	registerMethod("findPayloadType", make_method(this, &CoreAPI::findPayloadType));
	registerMethod("getPayloadTypeBitrate", make_method(this, &CoreAPI::getPayloadTypeBitrate));
	registerMethod("setPayloadTypeBitrate", make_method(this, &CoreAPI::setPayloadTypeBitrate));

	// ProxyConfig bindings
	registerMethod("addProxyConfig", make_method(this, &CoreAPI::addProxyConfig));
	registerMethod("clearProxyConfig", make_method(this, &CoreAPI::clearProxyConfig));
	registerMethod("createProxyConfig", make_method(this, &CoreAPI::createProxyConfig));
	registerMethod("removeProxyConfig", make_method(this, &CoreAPI::removeProxyConfig));
	registerProperty("proxyConfigList", make_property(this, &CoreAPI::getProxyConfigList));
	registerProperty("defaultProxy", make_property(this, &CoreAPI::getDefaultProxy, &CoreAPI::setDefaultProxy));
	registerMethod("refreshRegisters", make_method(this, &CoreAPI::refreshRegisters));
	registerProperty("primaryContact", make_property(this, &CoreAPI::getPrimaryContact, &CoreAPI::setPrimaryContact));

	// CallLog bindings
	registerMethod("clearCallLogs", make_method(this, &CoreAPI::clearCallLogs));
	registerProperty("callLogs", make_property(this, &CoreAPI::getCallLogs));
	registerMethod("removeCallLog", make_method(this, &CoreAPI::removeCallLog));

	// Network bindings
	registerProperty("audioPort", make_property(this, &CoreAPI::getAudioPort, &CoreAPI::setAudioPort));
	registerProperty("audioPortRange", make_property(this, &CoreAPI::getAudioPortRange, &CoreAPI::setAudioPortRange));
	registerProperty("videoPort", make_property(this, &CoreAPI::getVideoPort, &CoreAPI::setVideoPort));
	registerProperty("videoPortRange", make_property(this, &CoreAPI::getVideoPortRange, &CoreAPI::setVideoPortRange));
	registerProperty("downloadBandwidth", make_property(this, &CoreAPI::getDownloadBandwidth, &CoreAPI::setDownloadBandwidth));
	registerProperty("uploadBandwidth", make_property(this, &CoreAPI::getUploadBandwidth, &CoreAPI::setUploadBandwidth));
	registerProperty("dnsSrvEnabled", make_property(this, &CoreAPI::dnsSrvEnabled, &CoreAPI::enableDnsSrv));
	registerProperty("downloadPtime", make_property(this, &CoreAPI::getDownloadPtime, &CoreAPI::setDownloadPtime));
	registerProperty("uploadPtime", make_property(this, &CoreAPI::getUploadPtime, &CoreAPI::setUploadPtime));
	registerProperty("mtu", make_property(this, &CoreAPI::getMtu, &CoreAPI::setMtu));
	registerProperty("stunServer", make_property(this, &CoreAPI::getStunServer, &CoreAPI::setStunServer));
	registerProperty("natAddress", make_property(this, &CoreAPI::getNatAddress, &CoreAPI::setNatAddress));
	registerProperty("guessHostname", make_property(this, &CoreAPI::getGuessHostname, &CoreAPI::setGuessHostname));
	registerProperty("ipv6Enabled", make_property(this, &CoreAPI::ipv6Enabled, &CoreAPI::enableIpv6));
	registerProperty("keepAliveEnabled", make_property(this, &CoreAPI::keepAliveEnabled, &CoreAPI::enableKeepAlive));
	registerProperty("audioDscp", make_property(this, &CoreAPI::getAudioDscp, &CoreAPI::setAudioDscp));
	registerProperty("sipDscp", make_property(this, &CoreAPI::getSipDscp, &CoreAPI::setSipDscp));
	registerProperty("videoDscp", make_property(this, &CoreAPI::getVideoDscp, &CoreAPI::setVideoDscp));
	registerProperty("sipPort", make_property(this, &CoreAPI::getSipPort, &CoreAPI::setSipPort));
	registerProperty("sipTransports", make_property(this, &CoreAPI::getSipTransports, &CoreAPI::setSipTransports));
	registerProperty("adaptiveRateControlEnabled", make_property(this, &CoreAPI::adaptiveRateControlEnabled, &CoreAPI::enableAdaptiveRateControl));
	registerProperty("networkReachable", make_property(this, &CoreAPI::isNetworkReachable, &CoreAPI::setNetworkReachable));
	registerProperty("audioAdaptiveJittcompEnabled", make_property(this, &CoreAPI::audioAdaptiveJittcompEnabled, &CoreAPI::enableAudioAdaptiveJittcomp));
	registerProperty("videoAdaptiveJittcompEnabled", make_property(this, &CoreAPI::videoAdaptiveJittcompEnabled, &CoreAPI::enableVideoAdaptiveJittcomp));
	registerProperty("audioJittcomp", make_property(this, &CoreAPI::getAudioJittcomp, &CoreAPI::setAudioJittcomp));
	registerProperty("videoJittcomp", make_property(this, &CoreAPI::getVideoJittcomp, &CoreAPI::setVideoJittcomp));
	registerProperty("firewallPolicy", make_property(this, &CoreAPI::getFirewallPolicy, &CoreAPI::setFirewallPolicy));
	registerProperty("mediaEncryption", make_property(this, &CoreAPI::getMediaEncryption, &CoreAPI::setMediaEncryption));
	registerProperty("mediaEncryptionMandatory", make_property(this, &CoreAPI::isMediaEncryptionMandatory, &CoreAPI::setMediaEncryptionMandatory));
	registerProperty("nortpTimeout", make_property(this, &CoreAPI::getNortpTimeout, &CoreAPI::setNortpTimeout));
	registerProperty("delayedTimeout", make_property(this, &CoreAPI::getDelayedTimeout, &CoreAPI::setDelayedTimeout));

	// AuthInfo bindings
	registerMethod("addAuthInfo", make_method(this, &CoreAPI::addAuthInfo));
	registerMethod("abortAuthentication", make_method(this, &CoreAPI::abortAuthentication));
	registerMethod("removeAuthInfo", make_method(this, &CoreAPI::removeAuthInfo));
	registerMethod("findAuthInfo", make_method(this, &CoreAPI::findAuthInfo));
	registerProperty("authInfoList", make_property(this, &CoreAPI::getAuthInfoList));
	registerMethod("clearAllAuthInfo", make_method(this, &CoreAPI::clearAllAuthInfo));

	// File bindings
	REGISTER_PROPERTY_FILE(CoreAPI, "remoteRingbackTone", getRemoteRingbackTone, setRemoteRingbackTone);
	REGISTER_PROPERTY_FILE(CoreAPI, "ring", getRing, setRing);
	REGISTER_PROPERTY_FILE(CoreAPI, "ringback", getRingback, setRingback);
	REGISTER_PROPERTY_FILE(CoreAPI, "playFile", getPlayFile, setPlayFile);
	REGISTER_PROPERTY_FILE(CoreAPI, "recordFile", getRecordFile, setRecordFile);
	REGISTER_PROPERTY_FILE(CoreAPI, "rootCa", getRootCa, setRootCa);
	REGISTER_PROPERTY_FILE(CoreAPI, "staticPicture", getStaticPicture, setStaticPicture);
	REGISTER_PROPERTY_FILE(CoreAPI, "userCertificatesPath", getUserCertificatesPath, setUserCertificatesPath);
	REGISTER_PROPERTY_FILE(CoreAPI, "zrtpSecretsFile", getZrtpSecretsFile, setZrtpSecretsFile);
	registerMethod("playLocal", make_method(this, &CoreAPI::playLocal));

	// Initiator bindings
	registerProperty("fileManager", make_property(this, &CoreAPI::getFileManager));
	registerMethod("newProxyConfig", make_method(this, &CoreAPI::newProxyConfig));
	registerMethod("newAuthInfo", make_method(this, &CoreAPI::newAuthInfo));
	registerMethod("newAddress", make_method(this, &CoreAPI::newAddress));
	registerMethod("createContent", make_method(this, &CoreAPI::createContent));
	registerMethod("newLpConfig", make_method(this, &CoreAPI::newLpConfig));
	registerMethod("newFriend", make_method(this, &CoreAPI::newFriend));
	registerMethod("newFriendWithAddress", make_method(this, &CoreAPI::newFriendWithAddress));
	registerMethod("newPresenceActivity", make_method(this, &CoreAPI::newPresenceActivity));
	registerMethod("newPresenceModel", make_method(this, &CoreAPI::newPresenceModel));
	registerMethod("newPresenceModelWithActivity", make_method(this, &CoreAPI::newPresenceModelWithActivity));
	registerMethod("newPresenceModelWithActivityAndNote", make_method(this, &CoreAPI::newPresenceModelWithActivityAndNote));
	registerMethod("newPresenceNote", make_method(this, &CoreAPI::newPresenceNote));
	registerMethod("newPresencePerson", make_method(this, &CoreAPI::newPresencePerson));
	registerMethod("newPresenceService", make_method(this, &CoreAPI::newPresenceService));
	registerMethod("newTunnelConfig", make_method(this, &CoreAPI::newTunnelConfig));

	// Dtmf
	registerMethod("sendDtmf", make_method(this, &CoreAPI::sendDtmf));
	registerMethod("playDtmf", make_method(this, &CoreAPI::playDtmf));
	registerMethod("stopDtmf", make_method(this, &CoreAPI::stopDtmf));
	registerProperty("useInfoForDtmf", make_property(this, &CoreAPI::getUseInfoForDtmf, &CoreAPI::setUseInfoForDtmf));
	registerProperty("useRfc2833ForDtmf", make_property(this, &CoreAPI::getUseRfc2833ForDtmf, &CoreAPI::setUseRfc2833ForDtmf));

	// Friend bindings
	registerMethod("addFriend", make_method(this, &CoreAPI::addFriend));
	registerMethod("getFriendByAddress", make_method(this, &CoreAPI::getFriendByAddress));
	registerMethod("getFriendByRefKey", make_method(this, &CoreAPI::getFriendByRefKey));
	registerProperty("friendList", make_property(this, &CoreAPI::getFriendList));
	registerMethod("rejectSubscriber", make_method(this, &CoreAPI::rejectSubscriber));
	registerMethod("removeFriend", make_method(this, &CoreAPI::removeFriend));

	// Presence
	registerProperty("presenceModel", make_property(this, &CoreAPI::getPresenceModel, &CoreAPI::setPresenceModel));
	registerMethod("notifyAllFriends", make_method(this, &CoreAPI::notifyAllFriends));

	// Miscs
	registerProperty("avpfMode", make_property(this, &CoreAPI::getAvpfMode, &CoreAPI::setAvpfMode));
	registerProperty("avpfRrInterval", make_property(this, &CoreAPI::getAvpfRrInterval, &CoreAPI::setAvpfRrInterval));
	registerProperty("echoCancellationEnabled", make_property(this, &CoreAPI::echoCancellationEnabled, &CoreAPI::enableEchoCancellation));
	registerProperty("echoLimiterEnabled", make_property(this, &CoreAPI::echoLimiterEnabled, &CoreAPI::enableEchoLimiter));
	registerProperty("staticPictureFps", make_property(this, &CoreAPI::getStaticPictureFps, &CoreAPI::setStaticPictureFps));
	registerMethod("interpretUrl", make_method(this, &CoreAPI::interpretUrl));

	// Log
	registerProperty("logHandler", make_property(this, &CoreAPI::getLogHandler, &CoreAPI::setLogHandler));

	// uPnP
	registerProperty("upnpAvailable", make_property(this, &CoreAPI::upnpAvailable));
	registerProperty("upnpExternalIpaddress", make_property(this, &CoreAPI::getUpnpExternalIpaddress));
	registerProperty("upnpState", make_property(this, &CoreAPI::getUpnpState));

	// Tunnel
	registerProperty("tunnelAvailable", make_property(this, &CoreAPI::tunnelAvailable));
	registerProperty("tunnel", make_property(this, &CoreAPI::getTunnel));
}

void CoreAPI::prepareInit() {
	FileManagerAPIPtr fm = getFileManager();
	FB::URI pluginsUri("internal:///lib/mediastreamer/plugins");
	std::string pluginsDir = fm->uriToFile(pluginsUri);
	ms_base_init();
	ms_voip_init();
	ms_set_plugins_dir(pluginsDir.c_str());
	ms_plugins_init();

	// Initialize callback table
	memset(&mVtable, 0, sizeof(LinphoneCoreVTable));
	mVtable.global_state_changed = CoreAPI::wrapper_global_state_changed;
	mVtable.registration_state_changed = CoreAPI::wrapper_registration_state_changed;
	mVtable.call_state_changed = CoreAPI::wrapper_call_state_changed;
	mVtable.notify_presence_received = CoreAPI::wrapper_notify_presence_received;
	mVtable.new_subscription_requested = CoreAPI::wrapper_new_subscription_requested;
	mVtable.auth_info_requested = CoreAPI::wrapper_auth_info_requested;
	mVtable.call_log_updated = CoreAPI::wrapper_call_log_updated;
	mVtable.message_received = CoreAPI::wrapper_message_received;
	mVtable.is_composing_received = CoreAPI::wrapper_is_composing_received;
	mVtable.dtmf_received = CoreAPI::wrapper_dtmf_received;
	mVtable.refer_received = CoreAPI::wrapper_refer_received;
	mVtable.call_encryption_changed = CoreAPI::wrapper_call_encryption_changed;
	mVtable.transfer_state_changed = CoreAPI::wrapper_transfer_state_changed;
	mVtable.buddy_info_updated = CoreAPI::wrapper_buddy_info_updated;
	mVtable.call_stats_updated = CoreAPI::wrapper_call_stats_updated;
	mVtable.info_received = CoreAPI::wrapper_info_received;
	mVtable.subscription_state_changed = CoreAPI::wrapper_subscription_state_changed;
	mVtable.notify_received = CoreAPI::wrapper_notify_received;
	mVtable.publish_state_changed = CoreAPI::wrapper_publish_state_changed;
	mVtable.configuring_status = CoreAPI::wrapper_configuring_status;
	mVtable.file_transfer_recv = CoreAPI::wrapper_file_transfer_recv;
	mVtable.file_transfer_send = CoreAPI::wrapper_file_transfer_send;
	mVtable.file_transfer_progress_indication = CoreAPI::wrapper_file_transfer_progress_indication;
	mVtable.network_reachable = CoreAPI::wrapper_network_reachable;
	mVtable.log_collection_upload_state_changed = CoreAPI::wrapper_log_collection_upload_state_changed;
	mVtable.log_collection_upload_progress_indication = CoreAPI::wrapper_log_collection_upload_progress_indication;
}

void CoreAPI::finishInit() {
	FileManagerAPIPtr fm = getFileManager();

	// Specific Linphone Web behaviour
	linphone_core_set_native_preview_window_id(mCore, (void *)(unsigned long) -1); // MUST be set to -1, we can't allow a detached window
	linphone_core_set_native_video_window_id(mCore, (void *)(unsigned long) -1); // MUST be set to -1, we can't allow a detached window

	// Sound file paths
	FB::URI staticPictureUri("internal:///share/images/nowebcamCIF.jpg");
	std::string staticPictureFile = fm->uriToFile(staticPictureUri);
	linphone_core_set_static_picture(mCore, staticPictureFile.c_str());
	FB::URI ringUri("internal:///share/sounds/linphone/rings/oldphone.wav");
	std::string ringFile = fm->uriToFile(ringUri);
	linphone_core_set_ring(mCore, ringFile.c_str());
	FB::URI ringbackUri("internal:///share/sounds/linphone/ringback.wav");
	std::string ringbackFile = fm->uriToFile(ringbackUri);
	linphone_core_set_ringback(mCore, ringbackFile.c_str());
	FB::URI playFileUri("internal:///share/sounds/linphone/rings/toy-mono.wav");
	std::string playFileFile = fm->uriToFile(playFileUri);
	linphone_core_set_play_file(mCore, playFileFile.c_str());

	// Other file paths
	FB::URI rootCaUri("internal:///share/linphone/rootca.pem");
	std::string rootCaFile = fm->uriToFile(rootCaUri);
	linphone_core_set_root_ca(mCore, rootCaFile.c_str());
	FB::URI chatDatabaseUri("local:///chat_db");
	std::string chatDatabaseFile = fm->uriToFile(chatDatabaseUri);
	linphone_core_set_chat_database_path(mCore, chatDatabaseFile.c_str());
	FB::URI zrtpSecretsUri("local:///zrtp_secrets");
	std::string zrtpSecretsFile = fm->uriToFile(zrtpSecretsUri);
	linphone_core_set_zrtp_secrets_file(mCore, zrtpSecretsFile.c_str());
	FB::URI userCertificatesUri("local:///user_certificates");
	std::string userCertificatesPath = fm->uriToFile(userCertificatesUri);
	linphone_core_set_user_certificates_path(mCore, userCertificatesPath.c_str());
}

int CoreAPI::init(StringPtr const &config, StringPtr const &factory) {
	FBLOG_DEBUG("CoreAPI::init", "this=" << this << "\t" << "config=" << config << "\t" << "factory=" << factory);
	StringPtr realConfig = config;
	StringPtr realFactory = factory;
	GET_FILE("CoreAPI::init", realConfig);
	GET_FILE("CoreAPI::init", realFactory);

	if(mCore != NULL) {
		return -1;
	}

	prepareInit();
	mCore = linphone_core_new(&mVtable, STRING_TO_CHARPTR(realConfig), STRING_TO_CHARPTR(realFactory), (void *) this);
	if (linphone_core_get_user_data(mCore) != this) {
		FBLOG_ERROR("CoreAPI::init", "Too old version of linphone core!");
		return 1;
	}
	finishInit();

	return 0;
}

int CoreAPI::initFromConfig(LpConfigAPIPtr const &config) {
	FBLOG_DEBUG("CoreAPI::init", "this=" << this << "\t" << "config=" << config);

	if(mCore != NULL) {
		return -1;
	}

	prepareInit();
	mCore = linphone_core_new_with_config(&mVtable, config->getRef(), (void *) this);
	if (linphone_core_get_user_data(mCore) != this) {
		FBLOG_ERROR("CoreAPI::init", "Too old version of linphone core!");
		return 1;
	}
	finishInit();

	return 0;
}

int CoreAPI::uninit() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::uninit", "this=" << this);
	if (mCore != NULL) {
		enableIterate(false);
		linphone_core_set_user_data(mCore, NULL);
		linphone_core_destroy(mCore);
		mCore = NULL;
		ms_exit();
		return 0;
	}
	return -1;
}

void CoreAPI::enableIterate(bool enable) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableIterate", "this=" << this << "\t" << "enable=" << enable);

	if(enable == mIterate)
		return;

	if(enable) {
		mTimer->start();
		mIterate = true;
	} else {
		mTimer->stop();
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
	mTimer->stop();
	mTimer = FB::Timer::getTimer(mIterateInterval, true, boost::bind(&CoreAPI::iterateWithMutex, this));
	mTimer->start();
}

int CoreAPI::getIterateInterval() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getIterateInterval", "this=" << this);
	return mIterateInterval;
}

LpConfigAPIPtr CoreAPI::getConfig() const {
	FBLOG_DEBUG("CoreAPI::getConfig", "this=" << this);
	return getFactory()->getLpConfig(linphone_core_get_config(mCore));
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

StringPtr const &CoreAPI::getMagic() const {
	FBLOG_DEBUG("CoreAPI::getMagic", "this=" << this);
	return mMagic;
}

void CoreAPI::setMagic(StringPtr const &magic) {
	FBLOG_DEBUG("CoreAPI::setMagic", "this=" << this << "\t" << "magic=" << magic);
	mMagic = magic;
}

/*
 *
 * Call functions
 *
 */

CallAPIPtr CoreAPI::invite(StringPtr const &url) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::invite", "this=" << this << "\t" << "url=" << url);
	LinphoneCall *call = linphone_core_invite(mCore, STRING_TO_CHARPTR(url));
	CallAPIPtr shared_call = getFactory()->getCall(call);
	return shared_call;
}

CallAPIPtr CoreAPI::inviteAddress(AddressAPIPtr const &address) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::inviteAddress", "this=" << this << "\t" << "address=" << address);
	LinphoneCall *call = linphone_core_invite_address(mCore, address->getRef());
	CallAPIPtr shared_call = getFactory()->getCall(call);
	return shared_call;
}

CallAPIPtr CoreAPI::inviteWithParams(StringPtr const &url, CallParamsAPIPtr const &params) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::invite", "this=" << this << "\t" << "url=" << url << "\t" << "params=" << params);
	LinphoneCall *call = linphone_core_invite_with_params(mCore, STRING_TO_CHARPTR(url), params->getRef());
	CallAPIPtr shared_call = getFactory()->getCall(call);
	return shared_call;
}

CallAPIPtr CoreAPI::inviteAddressWithParams(AddressAPIPtr const &address, CallParamsAPIPtr const &params) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::inviteAddress", "this=" << this << "\t" << "address=" << address << "\t" << "params=" << params);
	LinphoneCall *call = linphone_core_invite_address_with_params(mCore, address->getRef(), params->getRef());
	CallAPIPtr shared_call = getFactory()->getCall(call);
	return shared_call;
}

int CoreAPI::acceptCall(CallAPIPtr const &call) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::acceptCall", "this=" << this << "\t" << "call=" << call);
	return linphone_core_accept_call(mCore, call->getRef());
}

int CoreAPI::acceptCallWithParams(CallAPIPtr const &call, CallParamsAPIPtr const &params) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::acceptCallWithParams", "this=" << this << "\t" << "call=" << call << "\t" << "params=" << params);
	return linphone_core_accept_call_with_params(mCore, call->getRef(), params->getRef());
}

int CoreAPI::acceptEarlyMedia(CallAPIPtr const &call) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::acceptEarlyMedia", "this=" << this << "\t" << "call=" << call);
	return linphone_core_accept_early_media(mCore, call->getRef());
}

int CoreAPI::acceptEarlyMediaWithParams(CallAPIPtr const &call, CallParamsAPIPtr const &params) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::acceptEarlyMediaWithParams", "this=" << this << "\t" << "call=" << call << "\t" << "params=" << params);
	return linphone_core_accept_early_media_with_params(mCore, call->getRef(), params->getRef());
}

CallAPIPtr CoreAPI::getCurrentCall() {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getCurrentCall", "this=" << this);
	return getFactory()->getCall(linphone_core_get_current_call(mCore));
}

int CoreAPI::terminateCall(CallAPIPtr const &call) {
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

int CoreAPI::redirectCall(CallAPIPtr const &call, StringPtr const &uri) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::redirectCall", "this=" << this << "\t" << "call=" << call << "\t" << "uri=" << uri);
	return linphone_core_redirect_call(mCore, call->getRef(), STRING_TO_CHARPTR(uri));
}

int CoreAPI::declineCall(CallAPIPtr const &call, int reason) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::declineCall", "this=" << this << "\t" << "call=" << call << "\t" << "reason=" << reason);
	return linphone_core_decline_call(mCore, call->getRef(), (LinphoneReason)reason);
}

int CoreAPI::transferCall(CallAPIPtr const &call, StringPtr const &uri) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::transferCall", "this=" << this << "\t" << "call=" << call << "\t" << "uri=" << uri);
	return linphone_core_transfer_call(mCore, call->getRef(), STRING_TO_CHARPTR(uri));
}

int CoreAPI::transferCallToAnother(CallAPIPtr const &call, CallAPIPtr const &dest) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::transferCallToAnother", "this=" << this << "\t" << "call=" << call << "\t" << "dest=" << dest);
	return linphone_core_transfer_call_to_another(mCore, call->getRef(), dest->getRef());
}

int CoreAPI::resumeCall(CallAPIPtr const &call) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::resumeCall", "this=" << this << "\t" << "call=" << call);
	return linphone_core_resume_call(mCore, call->getRef());
}

int CoreAPI::pauseCall(CallAPIPtr const &call) {
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

int CoreAPI::updateCall(CallAPIPtr const &call, CallParamsAPIPtr const &params) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::updateCall", "this=" << this << "\t" << "call=" << call << "\t" << "params=" << params);
	LinphoneCallParams *lcparams = (params == NULL) ? NULL : params->getRef();
	return linphone_core_update_call(mCore, call->getRef(), lcparams);
}

int CoreAPI::deferCallUpdate(CallAPIPtr const &call) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::deferCallUpdate", "this=" << this << "\t" << "call=" << call);
	return linphone_core_defer_call_update(mCore, call->getRef());
}

int CoreAPI::acceptCallUpdate(CallAPIPtr const &call, CallParamsAPIPtr const &params) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::acceptCallUpdate", "this=" << this << "\t" << "call=" << call << "\t" << "params=" << params);
	return linphone_core_accept_call_update(mCore, call->getRef(), params->getRef());
}

CallParamsAPIPtr CoreAPI::createCallParams(CallAPIPtr const &call) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::createCallParams", "this=" << this << "\t" << "call=" << call);
	return getFactory()->getCallParams(linphone_core_create_call_params(mCore, call->getRef()));
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

CallAPIPtr CoreAPI::findCallFromUri(StringPtr const &uri) const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::findCallFromUri", "this=" << this << "\t" << "uri=" << uri);
	return getFactory()->getCall(linphone_core_find_call_from_uri(mCore, STRING_TO_CHARPTR(uri)));
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

void CoreAPI::resetMissedCallsCount() {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::resetMissedCallsCount", "this=" << this);
	return linphone_core_reset_missed_calls_count(mCore);
}

bool CoreAPI::getRingDuringIncomingEarlyMedia() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getRingDuringIncomingEarlyMedia", "this=" << this);
	return linphone_core_get_ring_during_incoming_early_media(mCore) == TRUE ? true : false;
}

void CoreAPI::setRingDuringIncomingEarlyMedia(bool enable) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setRingDuringIncomingEarlyMedia", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_set_ring_during_incoming_early_media(mCore, enable ? TRUE : FALSE);
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

int CoreAPI::addToConference(CallAPIPtr const &call) {
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

int CoreAPI::removeFromConference(CallAPIPtr const &call) {
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
 * Chat functions
 *
 */

IMPLEMENT_PROPERTY_FILE(CoreAPI, getChatDatabasePath, setChatDatabasePath);

StringPtr CoreAPI::getChatDatabasePath() const {
	FBLOG_DEBUG("CoreAPI::getChatDatabasePath", "this=" << this);
	FBLOG_ERROR("CoreAPI::getChatDatabasePath", "NOT IMPLEMENTED");
	return StringPtr(); // TODO Don't have API yet
}

void CoreAPI::setChatDatabasePath(StringPtr const &path) {
	FB_ASSERT_CORE
	CORE_MUTEX
	FBLOG_DEBUG("CoreAPI::setChatDatabasePath", "this=" << this << "\t" << "path=" << path);
	linphone_core_set_chat_database_path(mCore, STRING_TO_CHARPTR(path));
}

StringPtr CoreAPI::getFileTransferServer() const {
	FB_ASSERT_CORE
	CORE_MUTEX
	FBLOG_DEBUG("CoreAPI::getFileTransferServer", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_file_transfer_server(mCore));
}

void CoreAPI::setFileTransferServer(StringPtr const &server) {
	FB_ASSERT_CORE
	CORE_MUTEX
	FBLOG_DEBUG("CoreAPI::setFileTransferServer", "this=" << this << "\t" << "server=" << server);
	linphone_core_set_file_transfer_server(mCore, STRING_TO_CHARPTR(server));
}

bool CoreAPI::chatEnabled() const {
	FB_ASSERT_CORE
	CORE_MUTEX
	FBLOG_DEBUG("CoreAPI::chatEnabled", "this=" << this);
	return (linphone_core_chat_enabled(mCore) == TRUE);
}

std::vector<ChatRoomAPIPtr> CoreAPI::getChatRooms() const {
	FB_ASSERT_CORE
	CORE_MUTEX
	FBLOG_DEBUG("CoreAPI::getChatRooms", "this=" << this);
	std::vector<ChatRoomAPIPtr> list;
	for (const MSList *node = linphone_core_get_chat_rooms(mCore); node != NULL; node = ms_list_next(node)) {
		list.push_back(getFactory()->getChatRoom(reinterpret_cast<LinphoneChatRoom *>(node->data)));
	}
	return list;
}

void CoreAPI::disableChat(int denyReason) {
	FB_ASSERT_CORE
	CORE_MUTEX
	FBLOG_DEBUG("CoreAPI::disableChat", "this=" << this << "\t" << "denyReason=" << denyReason);
	linphone_core_disable_chat(mCore, (LinphoneReason)denyReason);
}

void CoreAPI::enableChat() {
	FB_ASSERT_CORE
	CORE_MUTEX
	FBLOG_DEBUG("CoreAPI::enableChat", "this=" << this);
	linphone_core_enable_chat(mCore);
}

ChatRoomAPIPtr CoreAPI::getChatRoom(AddressAPIPtr const &addr) const {
	FB_ASSERT_CORE
	CORE_MUTEX
	FBLOG_DEBUG("CoreAPI::getChatRoom", "this=" << this << "\t" << "addr=" << addr);
	LinphoneChatRoom *chatRoom = linphone_core_get_chat_room(mCore, addr->getRef());
	return getFactory()->getChatRoom(chatRoom);
}

ChatRoomAPIPtr CoreAPI::getChatRoomFromUri(StringPtr const &to) const {
	FB_ASSERT_CORE
	CORE_MUTEX
	FBLOG_DEBUG("CoreAPI::getChatRoomFromUri", "this=" << this << "\t" << "to=" << to);
	LinphoneChatRoom *chatRoom = linphone_core_get_chat_room_from_uri(mCore, STRING_TO_CHARPTR(to));
	return getFactory()->getChatRoom(chatRoom);
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

void CoreAPI::enableMic(bool enable) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableMic", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_enable_mic(mCore, enable ? TRUE : FALSE);
}

bool CoreAPI::micEnabled() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::micEnabled", "this=" << this);
	return linphone_core_mic_enabled(mCore) == TRUE ? true : false;
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

void CoreAPI::enableVideoCapture(bool enable) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableVideoCapture", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_enable_video_capture(mCore, enable ? TRUE : FALSE);
}

bool CoreAPI::videoCaptureEnabled() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::videoCaptureEnabled", "this=" << this);
	return linphone_core_video_capture_enabled(mCore) == TRUE ? true : false;
}

void CoreAPI::enableVideoDisplay(bool enable) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableVideoDisplay", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_enable_video_display(mCore, enable ? TRUE : FALSE);
}

bool CoreAPI::videoDisplayEnabled() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::videoDisplayEnabled", "this=" << this);
	return linphone_core_video_display_enabled(mCore) == TRUE ? true : false;
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
	
void CoreAPI::videoWindowEventHandler(const CoreAPIWeakPtr &corePtr, void *ptr) {
	CoreAPIPtr core = corePtr.lock();
	if(core) {
		core->setVideoWindow(ptr);
	}
}
	
void CoreAPI::setVideoWindow(void *ptr) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setVideoWindow", "this=" << this << "\t" << "ptr=" << ptr);
	if(ptr != NULL) {
		linphone_core_set_native_video_window_id(mCore, ptr);
	} else {
		linphone_core_set_native_video_window_id(mCore, (void *)(unsigned long)-1);
	}
}

void CoreAPI::setNativeVideoWindowId(WhiteBoard::IdType id) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setNativeVideoWindowId", "this=" << this << "\t" << "id=" << id);
	
	if(getFactory()->getWhiteBoard()->getId(mVideoWindow) == id) {
		// Same do nothing
		return;
	}

	mVideoWindow = getFactory()->getWhiteBoard()->getValue<VideoAPIWeakPtr>(id);
	VideoAPIPtr video = mVideoWindow.lock();
	if(video) {
		video->setWindowEventHandler(boost::bind(&CoreAPI::videoWindowEventHandler,
										   CoreAPIWeakPtr(boost::static_pointer_cast<CoreAPI>(this->shared_from_this())),
										   _1));
	}
}

WhiteBoard::IdType CoreAPI::getNativeVideoWindowId() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getNativeVideoWindowId", "this=" << this);
	
	return getFactory()->getWhiteBoard()->getId(mVideoWindow);
}
	
void CoreAPI::previewWindowEventHandler(const CoreAPIWeakPtr &corePtr, void *ptr) {
	CoreAPIPtr core = corePtr.lock();
	if(core) {
		core->setPreviewWindow(ptr);
	}
}
	
void CoreAPI::setPreviewWindow(void *ptr) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setPreviewWindow", "this=" << this << "\t" << "ptr=" << ptr);
	if(ptr != NULL) {
		linphone_core_set_native_preview_window_id(mCore, ptr);
	} else {
		linphone_core_set_native_preview_window_id(mCore, (void *)(unsigned long)-1);
	}
}

void CoreAPI::setNativePreviewWindowId(WhiteBoard::IdType id) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setNativePreviewWindowId", "this=" << this << "\t" << "id=" << id);
	
	if(getFactory()->getWhiteBoard()->getId(mPreviewWindow) == id) {
		// Same do nothing
		return;
	}

	mPreviewWindow = getFactory()->getWhiteBoard()->getValue<VideoAPIWeakPtr>(id);
	VideoAPIPtr video = mPreviewWindow.lock();
	if(video) {
		video->setWindowEventHandler(boost::bind(&CoreAPI::previewWindowEventHandler,
										   CoreAPIWeakPtr(boost::static_pointer_cast<CoreAPI>(this->shared_from_this())),
										   _1));
	}
}

WhiteBoard::IdType CoreAPI::getNativePreviewWindowId() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getNativePreviewWindowId", "this=" << this);
	
	return getFactory()->getWhiteBoard()->getId(mPreviewWindow);
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

void CoreAPI::setVideoPolicy(VideoPolicyAPIPtr const &videoPolicy) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setVideoPolicy", "this=" << this << "\t" << "policy=" << videoPolicy);
	linphone_core_set_video_policy(mCore, videoPolicy->getRef());
}

MSVideoSizeAPIPtr CoreAPI::getPreferredVideoSize() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getPreferredVideoSize", "this=" << this);
	return getFactory()->getMSVideoSize(linphone_core_get_preferred_video_size(mCore));
}

void CoreAPI::setPreferredVideoSize(MSVideoSizeAPIPtr const &videoSize) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setPreferredVideoSize", "this=" << this << "\t" << "videoSize=" << videoSize);
	MSVideoSize vs;
	vs.width = videoSize->getWidth();
	vs.height = videoSize->getHeight();
	linphone_core_set_preferred_video_size(mCore, vs);
}

MSVideoSizeAPIPtr CoreAPI::getPreferredVideoSizeByName() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getPreferredVideoSizeByName", "this=" << this);
	FBLOG_ERROR("CoreAPI::getPreferredVideoSizeByName", "NOT IMPLEMENTED");
	return MSVideoSizeAPIPtr(); // TODO Don't have API yet
}

void CoreAPI::setPreferredVideoSizeByName(StringPtr const &videoSize) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setPreferredVideoSizeByName", "this=" << this << "\t" << "videoSize=" << videoSize);
	linphone_core_set_preferred_video_size_by_name(mCore, STRING_TO_CHARPTR(videoSize));
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

bool CoreAPI::soundDeviceCanCapture(StringPtr const &devid) const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::soundDeviceCanCapture", "this=" << this << "\t" << "devid=" << devid);
	return linphone_core_sound_device_can_capture(mCore, STRING_TO_CHARPTR(devid)) == TRUE ? true : false;
}

bool CoreAPI::soundDeviceCanPlayback(StringPtr const &devid) const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::soundDeviceCanPlayback", "this=" << this << "\t" << "devid=" << devid);
	return linphone_core_sound_device_can_playback(mCore, STRING_TO_CHARPTR(devid)) == TRUE ? true : false;
}

void CoreAPI::setRingerDevice(StringPtr const &devid) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setRingerDevice", "this=" << this << "\t" << "devid=" << devid);
	linphone_core_set_ringer_device(mCore, STRING_TO_CHARPTR(devid));
}

void CoreAPI::setPlaybackDevice(StringPtr const &devid) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setPlaybackDevice", "this=" << this << "\t" << "devid=" << devid);
	linphone_core_set_playback_device(mCore, STRING_TO_CHARPTR(devid));
}

void CoreAPI::setCaptureDevice(StringPtr const &devid) {
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

void CoreAPI::setVideoDevice(StringPtr const &devid) {
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

void CoreAPI::setAudioCodecs(std::vector<PayloadTypeAPIPtr> const &list) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::set_audio_codecs", "this=" << this << "\t" << "list.size()=" << list.size());
	MSList *mslist = NULL;
	for (auto it = list.begin(); it != list.end(); ++it) {
			mslist = ms_list_append(mslist, (*it)->getRef());
	}

	linphone_core_set_audio_codecs(mCore, mslist);
}

void CoreAPI::setVideoCodecs(std::vector<PayloadTypeAPIPtr> const &list) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setVideoCodecs", "this=" << this << "\t" << "list.size()=" << list.size());
	MSList *mslist = NULL;
	for (auto it = list.begin(); it != list.end(); ++it) {
		mslist = ms_list_append(mslist, (*it)->getRef());
	}

	linphone_core_set_video_codecs(mCore, mslist);
}

bool CoreAPI::payloadTypeEnabled(PayloadTypeAPIPtr const &payloadType) const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::payloadTypeEnabled", "this=" << this << "\t" << "payloadType=" << payloadType);
	return linphone_core_payload_type_enabled(mCore, payloadType->getRef()) == TRUE ? true : false;
}

void CoreAPI::enablePayloadType(PayloadTypeAPIPtr const &payloadType, bool enable) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enablePayloadType", "this=" << this << "\t" << "payloadType=" << payloadType << "\t" << "enable=" << enable);
	linphone_core_enable_payload_type(mCore, payloadType->getRef(), enable ? TRUE : FALSE);
}

PayloadTypeAPIPtr CoreAPI::findPayloadType(StringPtr const &type, int rate, int channels) const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::findPayloadType", "this=" << this << "\t" << "type=" << type << "\t" << "rate=" << rate << "\t" << "channels=" << channels);
	return getFactory()->getPayloadType(linphone_core_find_payload_type(mCore, STRING_TO_CHARPTR(type), rate, channels));
}

int CoreAPI::getPayloadTypeBitrate(PayloadTypeAPIPtr const &payloadType) const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getPayloadTypeBitrate", "this=" << this << "\t" << "payloadType=" << payloadType);
	return linphone_core_get_payload_type_bitrate(mCore, payloadType->getRef());
}

void CoreAPI::setPayloadTypeBitrate(PayloadTypeAPIPtr const &payloadType, int bitrate) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setPayloadTypeBitrate", "this=" << this << "\t" << "payloadType=" << payloadType << "\t" << "bitrate=" << bitrate);
	linphone_core_set_payload_type_bitrate(mCore, payloadType->getRef(), bitrate);
}

/*
 *
 * Proxy functions
 *
 */

int CoreAPI::addProxyConfig(ProxyConfigAPIPtr const &config) {
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

ProxyConfigAPIPtr CoreAPI::createProxyConfig() {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::createProxyConfig", "this=" << this);
	return getFactory()->getProxyConfig(linphone_core_create_proxy_config(mCore));
}

void CoreAPI::removeProxyConfig(ProxyConfigAPIPtr const &config) {
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

void CoreAPI::setDefaultProxy(ProxyConfigAPIPtr const &config) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setDefaultProxy", "this=" << this << "\t" << "config=" << config);
	config->disOwn();
	linphone_core_set_default_proxy_config(mCore, config->getRef());
}

ProxyConfigAPIPtr CoreAPI::getDefaultProxy() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getDefaultProxy", "this=" << this);
	LinphoneProxyConfig *ptr = linphone_core_get_default_proxy_config(mCore);
	if (ptr != NULL)
		return getFactory()->getProxyConfig(ptr);
	return ProxyConfigAPIPtr();
}

void CoreAPI::setPrimaryContact(StringPtr const &contact) {
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

void CoreAPI::removeCallLog(CallLogAPIPtr const &calllog) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getCallLogs", "this=" << this << "\t" << "calllog=" << calllog);
	linphone_core_remove_call_log(mCore, calllog->getRef());
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

static bool parseRange(std::string const &str, int &min, int &max) {
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

void CoreAPI::setAudioPortRange(StringPtr const &range) {
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

void CoreAPI::setVideoPortRange(StringPtr const &range) {
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

void CoreAPI::enableDnsSrv(bool enable) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enableDnsSrv", "this=" << this << "\t" << "enable=" << enable);
	linphone_core_enable_dns_srv(mCore, enable ? TRUE : FALSE);
}

bool CoreAPI::dnsSrvEnabled() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::dnsSrvEnabled", "this=" << this);
	return linphone_core_dns_srv_enabled(mCore) == TRUE ? true : false;
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

void CoreAPI::setStunServer(StringPtr const &server) {
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

void CoreAPI::setNatAddress(StringPtr const &address) {
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

void CoreAPI::setSipTransports(SipTransportsAPIPtr const &sipTransports) {
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

bool CoreAPI::isMediaEncryptionMandatory() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::isMediaEncryptionMandatory", "this=" << this);
	return linphone_core_is_media_encryption_mandatory(mCore) == TRUE? true : false;
}

void CoreAPI::setMediaEncryptionMandatory(bool mandatory) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setMediaEncryptionMandatory", "this=" << this << "\t" << "mandatory=" << mandatory);
	linphone_core_set_media_encryption_mandatory(mCore, mandatory? TRUE : FALSE);
}

int CoreAPI::getNortpTimeout() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getNortpTimeout", "this=" << this);
	return linphone_core_get_nortp_timeout(mCore);
}

void CoreAPI::setNortpTimeout(int timeout) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setNortpTimeout", "this=" << this << "\t" << "timeout=" << timeout);
	linphone_core_set_nortp_timeout(mCore, timeout);
}

int CoreAPI::getDelayedTimeout() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getDelayedTimeout", "this=" << this);
	return linphone_core_get_delayed_timeout(mCore);
}

void CoreAPI::setDelayedTimeout(int timeout) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setDelayedTimeout", "this=" << this << "\t" << "timeout=" << timeout);
	linphone_core_set_delayed_timeout(mCore, timeout);
}


/*
 *
 * AuthInfo functions
 *
 */

void CoreAPI::addAuthInfo(AuthInfoAPIPtr const &authInfo) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::addAuthInfo", "this=" << this << "\t" << "authInfo=" << authInfo);
	linphone_core_add_auth_info(mCore, authInfo->getRef());
}

void CoreAPI::abortAuthentication(AuthInfoAPIPtr const &authInfo) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::abortAuthentication", "this=" << this << "\t" << "authInfo=" << authInfo);
	linphone_core_abort_authentication(mCore, authInfo->getRef());
}

void CoreAPI::removeAuthInfo(AuthInfoAPIPtr const &authInfo) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::removeAuthInfo", "this=" << this << "\t" << "authInfo=" << authInfo);
	linphone_core_remove_auth_info(mCore, authInfo->getRef());
}

AuthInfoAPIPtr CoreAPI::findAuthInfo(StringPtr const &realm, StringPtr const &username, StringPtr const &domain) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::findAuthInfo", "this=" << this << "\t" << "realm=" << realm << "\t" << "username=" << username << "\t" << "domain=" << domain);
	const LinphoneAuthInfo* authInfo = linphone_core_find_auth_info(mCore, STRING_TO_CHARPTR(realm), STRING_TO_CHARPTR(username), STRING_TO_CHARPTR(domain));
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
	FBLOG_WARN("CoreAPI::newProxyConfig", "Deprecated! Use createProxyConfig instead.");
	return getFactory()->getProxyConfig();
}

AuthInfoAPIPtr CoreAPI::newAuthInfo(StringPtr const &username, StringPtr const &userid, StringPtr const &passwd, StringPtr const &ha1,
		StringPtr const &realm, StringPtr const &domain) const {
	FBLOG_DEBUG("CoreAPI::newAuthInfo", "this=" << this
				<< "\t" << "username=" << username
				<< "\t" << "userid=" << userid
				<< "\t" << "passwd=" << passwd
				<< "\t" << "ha1=" << ha1
				<< "\t" << "realm=" << realm
				<< "\t" << "domain=" << domain);
	return getFactory()->getAuthInfo(username, userid, passwd, ha1, realm, domain);
}
	
AddressAPIPtr CoreAPI::newAddress(StringPtr const &address) const {
	FBLOG_DEBUG("CoreAPI::newAddress", "this=" << this << "\t" << "address=" << address);
	return getFactory()->getAddress(address);
}

ContentAPIPtr CoreAPI::createContent() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::createContent", "this=" << this);
	return getFactory()->getContent(linphone_core_create_content(mCore));
}

LpConfigAPIPtr CoreAPI::newLpConfig(StringPtr const &uri) const {
	FBLOG_DEBUG("CoreAPI::newLpConfig", "this=" << this << "\t" << "uri=" << uri);
	StringPtr filename;
	if (uri) {
		FB::URI fburi(*uri);
		filename = getFactory()->getFileManager()->uriToFile(fburi);
		if (filename->empty()) {
			FBLOG_WARN("CoreAPI::newLpConfig", "Invalid file URI: " << uri);
			throw FB::script_error(std::string("Invalid file URI: ") + PRINT_STRING(uri));
		}
	}
	return getFactory()->getLpConfig(filename);
}

FriendAPIPtr CoreAPI::newFriend() const {
	FBLOG_DEBUG("CoreAPI::newFriend", "this=" << this);
	return getFactory()->getFriend();
}

FriendAPIPtr CoreAPI::newFriendWithAddress(StringPtr const &address) const {
	FBLOG_DEBUG("CoreAPI::newFriend", "this=" << this << "\t" << "address=" << address);
	return getFactory()->getFriend(address);
}

PresenceActivityAPIPtr CoreAPI::newPresenceActivity(int type, StringPtr const &description) const {
	FBLOG_DEBUG("CoreAPI::newPresenceActivity", "this=" << this << "\t" << "type=" << type << "\t" << "description=" << description);
	return getFactory()->getPresenceActivity(type, description);
}

PresenceModelAPIPtr CoreAPI::newPresenceModel() const {
	FBLOG_DEBUG("CoreAPI::newPresenceModel", "this=" << this);
	return getFactory()->getPresenceModel();
}

PresenceModelAPIPtr CoreAPI::newPresenceModelWithActivity(int acttype, StringPtr const &description) const {
	FBLOG_DEBUG("CoreAPI::newPresenceModel", "this=" << this << "\t" << "acttype=" << acttype << "\t" << "description=" << description);
	return getFactory()->getPresenceModel(acttype, description);
}

PresenceModelAPIPtr CoreAPI::newPresenceModelWithActivityAndNote(int acttype, StringPtr const &description, StringPtr const &note, StringPtr const &lang) const {
	FBLOG_DEBUG("CoreAPI::newPresenceModel", "this=" << this << "\t" << "acttype=" << acttype << "\t" << "description=" << description << "\t" << "note=" << note << "\t" << "lang=" << lang);
	return getFactory()->getPresenceModel(acttype, description, note, lang);
}

PresenceNoteAPIPtr CoreAPI::newPresenceNote(StringPtr const &content, StringPtr const &lang) const {
	FBLOG_DEBUG("CoreAPI::newPresenceNote", "this=" << this << "\t" << "content=" << content << "\t" << "lang=" << lang);
	return getFactory()->getPresenceNote(content, lang);
}

PresencePersonAPIPtr CoreAPI::newPresencePerson(StringPtr const &id) const {
	FBLOG_DEBUG("CoreAPI::newPresencePerson", "this=" << this << "\t" << "id=" << id);
	return getFactory()->getPresencePerson(id);
}

PresenceServiceAPIPtr CoreAPI::newPresenceService(StringPtr const &id, int basicStatus, StringPtr const &contact) const {
	FBLOG_DEBUG("CoreAPI::newPresenceService", "this=" << this << "\t" << "id=" << id << "\t" << "basicStatus=" << basicStatus << "\t" << "contact=" << contact);
	return getFactory()->getPresenceService(id, basicStatus, contact);
}

TunnelConfigAPIPtr CoreAPI::newTunnelConfig() const {
	FBLOG_DEBUG("CoreAPI::newTunnelConfig", "this=" << this);
	return getFactory()->getTunnelConfig();
}


/*
 *
 * DTMF functions
 *
 */

void CoreAPI::sendDtmf(std::string const &dtmf) {
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

void CoreAPI::playDtmf(std::string const &dtmf, int duration_ms) {
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
 * Friend functions
 *
 */

void CoreAPI::addFriend(FriendAPIPtr const &f) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::addFriend", "this=" << this << "\t" << "f=" << f);
	linphone_core_add_friend(mCore, f->getRef());
	f->disOwn();
}

void CoreAPI::removeFriend(FriendAPIPtr const &f) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::removeFriend", "this=" << this << "\t" << "f=" << f);
	linphone_core_remove_friend(mCore, f->getRef());
}

void CoreAPI::rejectSubscriber(FriendAPIPtr const &f) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::rejectSubscriber", "this=" << this << "\t" << "f=" << f);
	linphone_core_reject_subscriber(mCore, f->getRef());
}

FriendAPIPtr CoreAPI::getFriendByAddress(StringPtr const &address) const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getFriendByAddress", "this=" << this << "\t" << "address=" << address);
	LinphoneFriend *f = linphone_core_get_friend_by_address(mCore, STRING_TO_CHARPTR(address));
	return getFactory()->getFriend(f);
}

FriendAPIPtr CoreAPI::getFriendByRefKey(StringPtr const &key) const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getFriendByRefKey", "this=" << this << "\t" << "key=" << key);
	LinphoneFriend *f = linphone_core_get_friend_by_ref_key(mCore, STRING_TO_CHARPTR(key));
	return getFactory()->getFriend(f);
}

std::vector<FriendAPIPtr> CoreAPI::getFriendList() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getFriendList", "this=" << this);
	std::vector<FriendAPIPtr> list;
	for (const MSList *node = linphone_core_get_friend_list(mCore); node != NULL; node = ms_list_next(node)) {
		list.push_back(getFactory()->getFriend(reinterpret_cast<LinphoneFriend *>(node->data)));
	}
	return list;
}


/*
 *
 * Presence functions
 *
 */

PresenceModelAPIPtr CoreAPI::getPresenceModel() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getPresenceModel", "this=" << this);
	LinphonePresenceModel *model = linphone_core_get_presence_model(mCore);
	return getFactory()->getPresenceModel(model);
}

void CoreAPI::setPresenceModel(PresenceModelAPIPtr const &model) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setPresenceModel", "this=" << this << "\t" << "model=" << model);
	linphone_core_set_presence_model(mCore, model->getRef());
}

void CoreAPI::notifyAllFriends(PresenceModelAPIPtr const &model) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::notifyAllFriends", "this=" << this << "\t" << "model=" << model);
	linphone_core_notify_all_friends(mCore, model->getRef());
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
	return CHARPTR_TO_STRING(FBSTRING_PLUGIN_GIT_REVISION);
}

StringPtr CoreAPI::getDepsVersion() const {
	FBLOG_DEBUG("CoreAPI::getDepsVersion", "this=" << this);
	FBLOG_WARN("CoreAPI::getDepsVersion", "Deprecated!");
	return CHARPTR_TO_STRING("");
}

StringPtr CoreAPI::getRootfsVersion() const {
	FBLOG_DEBUG("CoreAPI::getRootfsVersion", "this=" << this);
	FBLOG_WARN("CoreAPI::getRootfsVersion", "Deprecated!");
	return CHARPTR_TO_STRING("");
}

int CoreAPI::getAvpfMode() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getAvpfMode", "this=" << this);
	return (int)linphone_core_get_avpf_mode(mCore);
}

void CoreAPI::setAvpfMode(int mode) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setAvpfMode", "this=" << this << "\t" << "mode=" << mode);
	linphone_core_set_avpf_mode(mCore, (LinphoneAVPFMode)mode);
}

int CoreAPI::getAvpfRrInterval() const {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getAvpfRrInterval", "this=" << this);
	return linphone_core_get_avpf_rr_interval(mCore);
}

void CoreAPI::setAvpfRrInterval(int interval) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setAvpfRrInterval", "this=" << this << "\t" << "interval=" << interval);
	linphone_core_set_avpf_rr_interval(mCore, interval);
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

AddressAPIPtr CoreAPI::interpretUrl(StringPtr const &url) const {
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

IMPLEMENT_PROPERTY_FILE(CoreAPI, getRemoteRingbackTone, setRemoteRingbackTone);

StringPtr CoreAPI::getRemoteRingbackTone() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getRemoteRingbackTone", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_remote_ringback_tone(mCore));
}

void CoreAPI::setRemoteRingbackTone(StringPtr const &ringback) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setRemoteRingbackTone", "this=" << this << "\t" << "ringback=" << ringback);
	linphone_core_set_remote_ringback_tone(mCore, STRING_TO_CHARPTR(ringback));
}

IMPLEMENT_PROPERTY_FILE(CoreAPI, getRing, setRing);

StringPtr CoreAPI::getRing() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getRing", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_ring(mCore));
}

void CoreAPI::setRing(StringPtr const &ring) {
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

void CoreAPI::setRingback(StringPtr const &ringback) {
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
	return CHARPTR_TO_STRING(linphone_core_get_play_file(mCore));
}

void CoreAPI::setPlayFile(StringPtr const &playFile) {
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
	return CHARPTR_TO_STRING(linphone_core_get_record_file(mCore));
}

void CoreAPI::setRecordFile(StringPtr const &recordFile) {
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

void CoreAPI::setRootCa(StringPtr const &rootCa) {
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

void CoreAPI::setStaticPicture(StringPtr const &staticPicture) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setStaticPicture", "this=" << this << "\t" << "staticPicture=" << staticPicture);
	linphone_core_set_static_picture(mCore, STRING_TO_CHARPTR(staticPicture));
}

IMPLEMENT_PROPERTY_FILE(CoreAPI, getUserCertificatesPath, setUserCertificatesPath);

StringPtr CoreAPI::getUserCertificatesPath() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getUserCertificatesPath", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_user_certificates_path(mCore));
}

void CoreAPI::setUserCertificatesPath(StringPtr const &userCertificatesPath) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setUserCertificatesPath", "this=" << this << "\t" << "userCertificatesPath=" << userCertificatesPath);
	linphone_core_set_user_certificates_path(mCore, STRING_TO_CHARPTR(userCertificatesPath));
}

IMPLEMENT_PROPERTY_FILE(CoreAPI, getZrtpSecretsFile, setZrtpSecretsFile);

StringPtr CoreAPI::getZrtpSecretsFile() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getZrtpSecretsFile", "this=" << this);
	return CHARPTR_TO_STRING(linphone_core_get_zrtp_secrets_file(mCore));
}

void CoreAPI::setZrtpSecretsFile(StringPtr const &secretsFile) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::setZrtpSecretsFile", "this=" << this << "\t" << "secretsFile=" << secretsFile);
	linphone_core_set_zrtp_secrets_file(mCore, STRING_TO_CHARPTR(secretsFile));
}

void CoreAPI::playLocal(StringPtr const &audiofile) {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::playLocal", "this=" << this << "\t" << "audiofile=" << audiofile);
	StringPtr realAudiofile = audiofile;
	GET_FILE("CoreAPI::playLocal", realAudiofile);
	linphone_core_play_local(mCore, STRING_TO_CHARPTR(realAudiofile));
}


///////////////////////////////////////////////////////////////////////////////
// Logs
///////////////////////////////////////////////////////////////////////////////

void CoreAPI::setLogHandler(FB::JSObjectPtr const &handler) {	
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

TunnelAPIPtr CoreAPI::getTunnel() const {
	FB_ASSERT_CORE
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getTunnel", "this=" << this);
	LinphoneTunnel *t = linphone_core_get_tunnel(mCore);
	return getFactory()->getTunnel(t);
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
	if (cstate == LinphoneCallUpdatedByRemote) {
		linphone_core_defer_call_update(mCore, call);
	}
	fire_callStateChanged(boost::static_pointer_cast<CoreAPI>(shared_from_this()), getFactory()->getCall(call), cstate, CHARPTR_TO_STRING(message));
}

void CoreAPI::onNotifyPresenceReceived(LinphoneFriend * lf) {
	FBLOG_DEBUG("CoreAPI::onNotifyPresenceReceived",  "this=" << this << "\t" << "lf=" << lf);
	fire_notifyPresenceReceived(boost::static_pointer_cast<CoreAPI>(shared_from_this()), getFactory()->getFriend(lf));
}

void CoreAPI::onNewSubscriptionRequested(LinphoneFriend *lf, const char *url) {
	FBLOG_DEBUG("CoreAPI::onNewSubscriptionRequested",  "this=" << this << "\t" << "lf=" << lf << "\t" << "url=" << url);
	fire_newSubscriptionRequested(boost::static_pointer_cast<CoreAPI>(shared_from_this()), getFactory()->getFriend(lf), CHARPTR_TO_STRING(url));
}

void CoreAPI::onAuthInfoRequested(const char *realm, const char *username, const char *domain) {
	FBLOG_DEBUG("CoreAPI::onAuthInfoRequested",  "this=" << this << "\t" << "realm=" << realm << "\t" << "username=" << username << "\t" << "domain=" << domain);
	fire_authInfoRequested(boost::static_pointer_cast<CoreAPI>(shared_from_this()), CHARPTR_TO_STRING(realm), CHARPTR_TO_STRING(username), CHARPTR_TO_STRING(domain));
}

void CoreAPI::onCallLogUpdated(LinphoneCallLog *newcl) {
	FBLOG_DEBUG("CoreAPI::onCallLogUpdated",  "this=" << this << "\t" << "newcl=" << newcl);
	fire_callLogUpdated(boost::static_pointer_cast<CoreAPI>(shared_from_this()), getFactory()->getCallLog(newcl));
}

void CoreAPI::onMessageReceived(LinphoneChatRoom *room, LinphoneChatMessage *message) {
	FBLOG_DEBUG("CoreAPI::onMessageReceived", "this=" << this << "\t" << "room=" << room << "\t" << "message=" << message);
	fire_messageReceived(boost::static_pointer_cast<CoreAPI>(shared_from_this()), getFactory()->getChatRoom(room), getFactory()->getChatMessage(message));
}

void CoreAPI::onIsComposingReceived(LinphoneChatRoom *room) {
	FBLOG_DEBUG("CoreAPI::onIsComposingReceived", "this=" << this << "\t" << "room=" << room);
	fire_isComposingReceived(boost::static_pointer_cast<CoreAPI>(shared_from_this()), getFactory()->getChatRoom(room));
}

void CoreAPI::onDtmfReceived(LinphoneCall *call, int dtmf) {
	FBLOG_DEBUG("CoreAPI::onDtmfReceived",  "this=" << this << "\t" << "call=" << call << "\t" << "dtmf=" << dtmf);
	fire_dtmfReceived(boost::static_pointer_cast<CoreAPI>(shared_from_this()), getFactory()->getCall(call), dtmf);
}

void CoreAPI::onReferReceived(const char *refer_to) {
	FBLOG_DEBUG("CoreAPI::onReferReceived",  "this=" << this << "\t" << "refer_to=" << refer_to);
	fire_referReceived(boost::static_pointer_cast<CoreAPI>(shared_from_this()), CHARPTR_TO_STRING(refer_to));
}

void CoreAPI::onCallEncryptionChanged(LinphoneCall *call, bool_t on, const char *authentication_token) {
	FBLOG_DEBUG("CoreAPI::onCallEncryptionChanged",  "this=" << this << "\t" << "call=" << call << "\t" << "on=" << on << "\t" << "authentication_token=" << authentication_token);
	fire_callEncryptionChanged(boost::static_pointer_cast<CoreAPI>(shared_from_this()), getFactory()->getCall(call), (on == TRUE) ? true : false, CHARPTR_TO_STRING(authentication_token));
}

void CoreAPI::onTransferStateChanged(LinphoneCall *call, LinphoneCallState state) {
	FBLOG_DEBUG("CoreAPI::onCallEncryptionChanged",  "this=" << this << "\t" << "call=" << call << "\t" << "state=" << state);
	fire_transferStateChanged(boost::static_pointer_cast<CoreAPI>(shared_from_this()), getFactory()->getCall(call), state);
}

void CoreAPI::onBuddyInfoUpdated(LinphoneFriend *lf) {
	FBLOG_DEBUG("CoreAPI::onBuddyInfoUpdated",  "this=" << this << "\t" << "lf=" << lf);
	fire_buddyInfoUpdated(boost::static_pointer_cast<CoreAPI>(shared_from_this()), getFactory()->getFriend(lf));
}

void CoreAPI::onCallStatsUpdated(LinphoneCall *call, const LinphoneCallStats *stats) {
	FBLOG_DEBUG("CoreAPI::onCallStatsUpdated",  "this=" << this << "\t" << "call=" << call << "\t" << "stats=" << stats);
	fire_callStatsUpdated(boost::static_pointer_cast<CoreAPI>(shared_from_this()), getFactory()->getCall(call), getFactory()->getCallStats(stats));
}

void CoreAPI::onInfoReceived(LinphoneCall *call, const LinphoneInfoMessage *info) {
	FBLOG_DEBUG("CoreAPI::onInfoReceived",  "this=" << this << "\t" << "call=" << call << "\t" << "info=" << info);
}

void CoreAPI::onSubscriptionStateChanged(LinphoneEvent *event, LinphoneSubscriptionState state) {
	FBLOG_DEBUG("CoreAPI::onSubscriptionStateChanged",  "this=" << this << "\t" << "event=" << event << "\t" << "state=" << state);
}

void CoreAPI::onNotifyReceived(LinphoneEvent *event, const char *notified_event, const LinphoneContent *body) {
	FBLOG_DEBUG("CoreAPI::onNotifyReceived",  "this=" << this << "\t" << "event=" << event << "\t" << "notified_event=" << notified_event << "\t" << "body=" << body);
}

void CoreAPI::onPublishStateChanged(LinphoneEvent *event, LinphonePublishState state) {
	FBLOG_DEBUG("CoreAPI::onPublishStateChanged",  "this=" << this << "\t" << "event=" << event << "\t" << "state=" << state);
}

void CoreAPI::onConfiguringStatus(LinphoneConfiguringState status, const char *message) {
	FBLOG_DEBUG("CoreAPI::onConfiguringStatus", "this=" << this << "\t" << "status=" << status << "\t" << "message=" << (message ? message : ""));
	fire_configuringStatus(boost::static_pointer_cast<CoreAPI>(shared_from_this()), status, CHARPTR_TO_STRING(message));
}

void CoreAPI::onFileTransferRecv(LinphoneChatMessage *message, const LinphoneContent* content, const char* buff, size_t size) {
	FBLOG_DEBUG("CoreAPI::onFileTransferRecv", "this=" << this << "\t" << "message=" << message << "\t" << "content=" << content << "\t" << "buff=" << buff << "\t" << "size=" << size);
}

void CoreAPI::onFileTransferSend(LinphoneChatMessage *message,  const LinphoneContent* content, char* buff, size_t* size) {
	FBLOG_DEBUG("CoreAPI::onFileTransferSend", "this=" << this << "\t" << "message=" << message << "\t" << "content=" << content << "\t" << "buff=" << buff << "\t" << "size=" << size);
}

void CoreAPI::onFileTransferProgressIndication(LinphoneChatMessage *message, const LinphoneContent* content, size_t offset, size_t total) {
	FBLOG_DEBUG("CoreAPI::onFileTransferProgressIndication", "this=" << this << "\t" << "message=" << message << "\t" << "content=" << content << "\t" << "offset=" << offset << "\t" << "total=" << total);
}

void CoreAPI::onNetworkReachable(bool_t reachable) {
	FBLOG_DEBUG("CoreAPI::onNeworkReachable", "this=" << this << "\t" << "reachable=" << reachable);
	fire_networkReachable(boost::static_pointer_cast<CoreAPI>(shared_from_this()), (reachable == TRUE));
}

void CoreAPI::onLogCollectionUploadStateChanged(LinphoneCoreLogCollectionUploadState state, const char *info) {
	FBLOG_DEBUG("CoreAPI::onLogCollectionUploadStateChanged", "this=" << this << "\t" << "state=" << state << "\t" << "info=" << info);
	fire_logCollectionUploadStateChanged(boost::static_pointer_cast<CoreAPI>(shared_from_this()), state, CHARPTR_TO_STRING(info));
}

void CoreAPI::onLogCollectionUploadProgressIndication(size_t offset, size_t total) {
	FBLOG_DEBUG("CoreAPI::onLogCollectionUploadProgressIndication", "this=" << this << "\t" << "offset=" << offset << "\t" << "total=" << total);
	fire_logCollectionUploadProgressIndication(boost::static_pointer_cast<CoreAPI>(shared_from_this()), offset, total);
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
		FBLOG_ERROR("CoreAPI::wrapper_global_state_changed", "No proxy defined!");
	}
}
void CoreAPI::wrapper_registration_state_changed(LinphoneCore *lc, LinphoneProxyConfig *cfg, LinphoneRegistrationState cstate, const char *message) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onRegistrationStateChanged(cfg, cstate, message);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_registration_state_changed", "No proxy defined!");
	}
}
void CoreAPI::wrapper_call_state_changed(LinphoneCore *lc, LinphoneCall *call, LinphoneCallState cstate, const char *message) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onCallStateChanged(call, cstate, message);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_call_state_changed", "No proxy defined!");
	}
}
void CoreAPI::wrapper_notify_presence_received(LinphoneCore *lc, LinphoneFriend * lf) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onNotifyPresenceReceived(lf);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_notify_presence_received", "No proxy defined!");
	}
}
void CoreAPI::wrapper_new_subscription_requested(LinphoneCore *lc, LinphoneFriend *lf, const char *url) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onNewSubscriptionRequested(lf, url);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_new_subscription_requested", "No proxy defined!");
	}
}
void CoreAPI::wrapper_auth_info_requested(LinphoneCore *lc, const char *realm, const char *username, const char*domain) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onAuthInfoRequested(realm, username, domain);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_auth_info_requested", "No proxy defined!");
	}
}
void CoreAPI::wrapper_call_log_updated(LinphoneCore *lc, LinphoneCallLog *newcl) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onCallLogUpdated(newcl);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_call_log_updated", "No proxy defined!");
	}
}
void CoreAPI::wrapper_message_received(LinphoneCore *lc, LinphoneChatRoom *room, LinphoneChatMessage *message) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onMessageReceived(room, message);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_message_received", "No proxy defined!");
	}
}
void CoreAPI::wrapper_is_composing_received(LinphoneCore *lc, LinphoneChatRoom *room) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onIsComposingReceived(room);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_is_composing_received", "No proxy defined!");
	}
}
void CoreAPI::wrapper_dtmf_received(LinphoneCore *lc, LinphoneCall *call, int dtmf) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onDtmfReceived(call, dtmf);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_dtmf_received", "No proxy defined!");
	}
}
void CoreAPI::wrapper_refer_received(LinphoneCore *lc, const char *refer_to) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onReferReceived(refer_to);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_refer_received", "No proxy defined!");
	}
}
void CoreAPI::wrapper_call_encryption_changed(LinphoneCore *lc, LinphoneCall *call, bool_t on, const char *authentication_token) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onCallEncryptionChanged(call, on, authentication_token);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_call_encryption_changed", "No proxy defined!");
	}
}
void CoreAPI::wrapper_transfer_state_changed(LinphoneCore *lc, LinphoneCall *call, LinphoneCallState state) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onTransferStateChanged(call, state);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_transfer_state_changed", "No proxy defined!");
	}
}
void CoreAPI::wrapper_buddy_info_updated(LinphoneCore *lc, LinphoneFriend *lf) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onBuddyInfoUpdated(lf);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_buddy_info_updated", "No proxy defined!");
	}
}
void CoreAPI::wrapper_call_stats_updated(LinphoneCore *lc, LinphoneCall *call, const LinphoneCallStats *stats) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onCallStatsUpdated(call, stats);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_call_stats_updated", "No proxy defined!");
	}
}
void CoreAPI::wrapper_info_received(LinphoneCore *lc, LinphoneCall *call, const LinphoneInfoMessage *info) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onInfoReceived(call, info);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_info_received", "No proxy defined!");
	}
}
void CoreAPI::wrapper_subscription_state_changed(LinphoneCore *lc, LinphoneEvent *event, LinphoneSubscriptionState state) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onSubscriptionStateChanged(event, state);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_subscription_state_changed", "No proxy defined!");
	}
}
void CoreAPI::wrapper_notify_received(LinphoneCore *lc, LinphoneEvent *event, const char *notified_event, const LinphoneContent *body) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onNotifyReceived(event, notified_event, body);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_notify_received", "No proxy defined!");
	}
}
void CoreAPI::wrapper_publish_state_changed(LinphoneCore *lc, LinphoneEvent *event, LinphonePublishState state) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onPublishStateChanged(event, state);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_publish_state_changed", "No proxy defined!");
	}
}
void CoreAPI::wrapper_configuring_status(LinphoneCore *lc, LinphoneConfiguringState status, const char *message) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onConfiguringStatus(status, message);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_configuring_status", "No proxy defined!");
	}
}
void CoreAPI::wrapper_file_transfer_recv(LinphoneCore *lc, LinphoneChatMessage *message, const LinphoneContent* content, const char* buff, size_t size) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onFileTransferRecv(message, content, buff, size);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_file_transfer_recv", "No proxy defined!");
	}
}
void CoreAPI::wrapper_file_transfer_send(LinphoneCore *lc, LinphoneChatMessage *message,  const LinphoneContent* content, char* buff, size_t* size) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onFileTransferSend(message, content, buff, size);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_file_transfer_send", "No proxy defined!");
	}
}
void CoreAPI::wrapper_file_transfer_progress_indication(LinphoneCore *lc, LinphoneChatMessage *message, const LinphoneContent* content, size_t offset, size_t total) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onFileTransferProgressIndication(message, content, offset, total);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_file_transfer_progress_indication", "No proxy defined!");
	}
}
void CoreAPI::wrapper_network_reachable(LinphoneCore *lc, bool_t reachable) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onNetworkReachable(reachable);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_network_reachable", "No proxy defined!");
	}
}
void CoreAPI::wrapper_log_collection_upload_state_changed(LinphoneCore *lc, LinphoneCoreLogCollectionUploadState state, const char *info) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onLogCollectionUploadStateChanged(state, info);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_log_collection_upload_state_changed", "No proxy defined!");
	}
}
void CoreAPI::wrapper_log_collection_upload_progress_indication(LinphoneCore *lc, size_t offset, size_t total) {
	if (GLC_DEFINED()) {
		GLC_THIS()->onLogCollectionUploadProgressIndication(offset, total);
	} else {
		FBLOG_ERROR("CoreAPI::wrapper_log_collection_upload_progress_indication", "No proxy defined!");
	}
}

} // LinphoneWeb
