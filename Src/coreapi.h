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

#ifndef H_COREAPI
#define H_COREAPI

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <Timer.h>
#include <variant_list.h>

#include <linphone/linphonecore.h>
#include "wrapperapi.h"

namespace LinphoneWeb {

FB_FORWARD_PTR(AddressAPI)
FB_FORWARD_PTR(AuthInfoAPI)
FB_FORWARD_PTR(CallAPI)
FB_FORWARD_PTR(CallLogAPI)
FB_FORWARD_PTR(CallParamsAPI)
FB_FORWARD_PTR(CallStatsAPI)
FB_FORWARD_PTR(CoreAPI)
FB_FORWARD_PTR(FileManagerAPI)
FB_FORWARD_PTR(FriendAPI)
FB_FORWARD_PTR(LpConfigAPI)
FB_FORWARD_PTR(PayloadTypeAPI)
FB_FORWARD_PTR(PointerAPI)
FB_FORWARD_PTR(PresenceActivityAPI)
FB_FORWARD_PTR(PresenceModelAPI)
FB_FORWARD_PTR(PresenceNoteAPI)
FB_FORWARD_PTR(PresencePersonAPI)
FB_FORWARD_PTR(PresenceServiceAPI)
FB_FORWARD_PTR(ProxyConfigAPI)
FB_FORWARD_PTR(SipTransportsAPI)
FB_FORWARD_PTR(VideoAPI)
FB_FORWARD_PTR(VideoPolicyAPI)

class CoreAPI: public WrapperAPI {
	friend class FactoryAPI;
public:
	CoreAPI();
	virtual ~CoreAPI();

	// Read-only property
	StringPtr getVersion() const;
	StringPtr getPluginVersion() const;
	StringPtr getDepsVersion() const;
	StringPtr getRootfsVersion() const;

	// Property
	StringPtr const &getMagic() const;
	void setMagic(StringPtr const &magic);

	// Call functions
	CallAPIPtr invite(StringPtr const &url);
	CallAPIPtr inviteAddress(AddressAPIPtr const &address);
	CallAPIPtr inviteWithParams(StringPtr const &url, CallParamsAPIPtr const &params);
	CallAPIPtr inviteAddressWithParams(AddressAPIPtr const &address, CallParamsAPIPtr const &params);
	int acceptCall(CallAPIPtr const &call);
	int acceptCallWithParams(CallAPIPtr const &call, CallParamsAPIPtr const &params);
	CallAPIPtr getCurrentCall();
	int terminateCall(CallAPIPtr const &call);
	int terminateAllCalls();
	int redirectCall(CallAPIPtr const &call, StringPtr const &uri);
	int declineCall(CallAPIPtr const &call, int reason);
	int transferCall(CallAPIPtr const &call, StringPtr const &uri);
	int transferCallToAnother(CallAPIPtr const &call, CallAPIPtr const &dest);
	int resumeCall(CallAPIPtr const &call);
	int pauseCall(CallAPIPtr const &call);
	int pauseAllCalls();
	int updateCall(CallAPIPtr const &call, CallParamsAPIPtr const &params);
	int deferCallUpdate(CallAPIPtr const &call);
	int acceptCallUpdate(CallAPIPtr const &call, CallParamsAPIPtr const &params);
	CallParamsAPIPtr createDefaultCallParameters();
	int getIncTimeout() const;
	void setIncTimeout(int timeout);
	int getInCallTimeout() const;
	void setInCallTimeout(int timeout);
	int getMaxCalls() const;
	void setMaxCalls(int max);
	std::vector<CallAPIPtr> getCalls() const;
	CallAPIPtr findCallFromUri(StringPtr const &uri) const;
	int getCallsNb() const;
	int getMissedCallsCount() const;
	void resetMissedCallsCount();

	// Conference functions
	int addAllToConference();
	int addToConference(CallAPIPtr const &call);
	int enterConference();
	int getConferenceSize() const;
	bool isInConference() const;
	int leaveConference();
	int removeFromConference(CallAPIPtr const &call);
	int terminateConference();

	// Level functions
	void setPlayLevel(int level);
	int getPlayLevel() const;
	void setRecLevel(int level);
	int getRecLevel() const;
	void setRingLevel(int level);
	int getRingLevel() const;
	void enableMic(bool muted);
	bool micEnabled() const;
	float getMicGainDb() const;
	void setMicGainDb(float gain);
	float getPlaybackGainDb() const;
	void setPlaybackGainDb(float gain);

	// Video functions
	bool videoSupported() const;
	void enableVideoCapture(bool enable);
	bool videoCaptureEnabled() const;
	void enableVideoDisplay(bool enable);
	bool videoDisplayEnabled() const;
	void enableVideoPreview(bool enable);
	bool videoPreviewEnabled() const;
	void enableSelfView(bool enable);
	bool selfViewEnabled() const;
	void setNativeVideoWindowId(WhiteBoard::IdType id);
	WhiteBoard::IdType getNativeVideoWindowId() const;
	void setNativePreviewWindowId(WhiteBoard::IdType id);
	WhiteBoard::IdType getNativePreviewWindowId() const;
	bool getUsePreviewWindow() const;
	void setUsePreviewWindow(bool enable);
	VideoPolicyAPIPtr getVideoPolicy() const;
	void setVideoPolicy(VideoPolicyAPIPtr const &videoPolicy);

	// Sound device functions
	void reloadSoundDevices();
	std::vector<StringPtr> getSoundDevices() const;
	bool soundDeviceCanCapture(StringPtr const &devid) const;
	bool soundDeviceCanPlayback(StringPtr const &devid) const;
	void setRingerDevice(StringPtr const &devid);
	void setPlaybackDevice(StringPtr const &devid);
	void setCaptureDevice(StringPtr const &devid);
	StringPtr getRingerDevice() const;
	StringPtr getPlaybackDevice() const;
	StringPtr getCaptureDevice() const;

	// Video device functions
	void reloadVideoDevices();
	std::vector<StringPtr> getVideoDevices() const;
	void setVideoDevice(StringPtr const &devid);
	StringPtr getVideoDevice() const;

	// Codecs functions
	std::vector<PayloadTypeAPIPtr> getAudioCodecs() const;
	std::vector<PayloadTypeAPIPtr> getVideoCodecs() const;
	void setAudioCodecs(std::vector<PayloadTypeAPIPtr> const &list);
	void setVideoCodecs(std::vector<PayloadTypeAPIPtr> const &list);
	bool payloadTypeEnabled(PayloadTypeAPIPtr const &payloadType) const;
	void enablePayloadType(PayloadTypeAPIPtr const &payloadType, bool enable);
	PayloadTypeAPIPtr findPayloadType(StringPtr const &type, int rate, int channels) const;


	// ProxyConfig functions
	int addProxyConfig(ProxyConfigAPIPtr const &config);
	void clearProxyConfig();
	ProxyConfigAPIPtr createProxyConfig();
	void removeProxyConfig(ProxyConfigAPIPtr const &config);
	std::vector<ProxyConfigAPIPtr> getProxyConfigList() const;
	void setDefaultProxy(ProxyConfigAPIPtr const &config);
	ProxyConfigAPIPtr getDefaultProxy() const;
	void setPrimaryContact(StringPtr const &contact);
	StringPtr getPrimaryContact() const;
	void refreshRegisters();

	// CallLog functions
	void clearCallLogs();
	std::vector<CallLogAPIPtr> getCallLogs() const;
	void removeCallLog(CallLogAPIPtr const &calllog);

	// Network functions
	void setAudioPort(int port);
	int getAudioPort() const;
	void setAudioPortRange(StringPtr const &range);
	StringPtr getAudioPortRange() const;
	void setVideoPort(int port);
	int getVideoPort() const;
	void setVideoPortRange(StringPtr const &range);
	StringPtr getVideoPortRange() const;
	void setDownloadBandwidth(int bandwidth);
	int getDownloadBandwidth() const;
	void setUploadBandwidth(int bandwidth);
	int getUploadBandwidth() const;
	void enableDnsSrv(bool enable);
	bool dnsSrvEnabled() const;
	void setDownloadPtime(int ptime);
	int getDownloadPtime() const;
	void setUploadPtime(int ptime);
	int getUploadPtime() const;
	void setMtu(int mtu);
	int getMtu() const;
	void setStunServer(StringPtr const &server);
	StringPtr getStunServer() const;
	void setNatAddress(StringPtr const &address);
	StringPtr getNatAddress() const;
	void setGuessHostname(bool guess);
	bool getGuessHostname() const;
	void enableIpv6(bool enable);
	bool ipv6Enabled() const;
	void enableKeepAlive(bool enable);
	bool keepAliveEnabled() const;
	void setAudioDscp(int port);
	int getAudioDscp() const;
	void setSipDscp(int port);
	int getSipDscp() const;
	void setVideoDscp(int port);
	int getVideoDscp() const;
	int getSipPort() const;
	void setSipPort(int port);
	void setSipTransports(SipTransportsAPIPtr const &sipTransports);
	SipTransportsAPIPtr getSipTransports() const;
	bool adaptiveRateControlEnabled() const;
	void enableAdaptiveRateControl(bool enable);
	bool isNetworkReachable() const;
	void setNetworkReachable(bool reachable);
	bool audioAdaptiveJittcompEnabled() const;
	void enableAudioAdaptiveJittcomp(bool enable);
	bool videoAdaptiveJittcompEnabled() const;
	void enableVideoAdaptiveJittcomp(bool enable);
	int getAudioJittcomp() const;
	void setAudioJittcomp(int comp);
	int getVideoJittcomp() const;
	void setVideoJittcomp(int comp);
	int getFirewallPolicy() const;
	void setFirewallPolicy(int policy);
	int getMediaEncryption() const;
	void setMediaEncryption(int encryption);
	bool isMediaEncryptionMandatory() const;
	void setMediaEncryptionMandatory(bool mandatory);
	int getNortpTimeout() const;
	void setNortpTimeout(int timeout);
	int getDelayedTimeout() const;
	void setDelayedTimeout(int timeout);

	// AuthInfo functions
	void addAuthInfo(AuthInfoAPIPtr const &authInfo);
	void abortAuthentication(AuthInfoAPIPtr const &authInfo);
	void removeAuthInfo(AuthInfoAPIPtr const &authInfo);
	AuthInfoAPIPtr findAuthInfo(StringPtr const &realm, StringPtr const &username, StringPtr const &domain);
	std::vector<AuthInfoAPIPtr> getAuthInfoList() const;
	void clearAllAuthInfo();

	// Instantiator functions
	FileManagerAPIPtr getFileManager() const;
	ProxyConfigAPIPtr newProxyConfig() const;
	AuthInfoAPIPtr newAuthInfo(StringPtr const &username, StringPtr const &userid,
			StringPtr const &passwd, StringPtr const &ha1, StringPtr const &realm, StringPtr const &domain) const;
	AddressAPIPtr newAddress(StringPtr const &address) const;
	LpConfigAPIPtr newLpConfig(StringPtr const &uri) const;
	FriendAPIPtr newFriend() const;
	FriendAPIPtr newFriendWithAddress(StringPtr const &address) const;
	PresenceActivityAPIPtr newPresenceActivity(int type, StringPtr const &description) const;
	PresenceModelAPIPtr newPresenceModel() const;
	PresenceModelAPIPtr newPresenceModelWithActivity(int acttype, StringPtr const &description) const;
	PresenceModelAPIPtr newPresenceModelWithActivityAndNote(int acttype, StringPtr const &description, StringPtr const &note, StringPtr const &lang) const;
	PresenceNoteAPIPtr newPresenceNote(StringPtr const &content, StringPtr const &lang) const;
	PresencePersonAPIPtr newPresencePerson(StringPtr const &id) const;
	PresenceServiceAPIPtr newPresenceService(StringPtr const &id, int basicStatus, StringPtr const &contact) const;

	// Dtmf
	void sendDtmf(std::string const &dtmf);
	void stopDtmf();
	void playDtmf(std::string const &dtmf, int duration_ms);
	bool getUseInfoForDtmf() const;
	void setUseInfoForDtmf(bool enable);
	bool getUseRfc2833ForDtmf() const;
	void setUseRfc2833ForDtmf(bool enable);

	// Friend
	void addFriend(FriendAPIPtr const &f);
	void removeFriend(FriendAPIPtr const &f);
	void rejectSubscriber(FriendAPIPtr const &f);
	FriendAPIPtr getFriendByAddress(StringPtr const &address) const;
	FriendAPIPtr getFriendByRefKey(StringPtr const &key) const;
	std::vector<FriendAPIPtr> getFriendList() const;

	// Presence
	PresenceModelAPIPtr getPresenceModel() const;
	void setPresenceModel(PresenceModelAPIPtr const &model);
	void notifyAllFriends(PresenceModelAPIPtr const &model);

	// Core helpers
	int init(StringPtr const &config, StringPtr const &factory);
	int initFromConfig(LpConfigAPIPtr const &config);
	int uninit();
	void setIterateInterval(int ms);
	int getIterateInterval() const;
	void enableIterate(bool enable);
	bool iterateEnabled() const;
	LpConfigAPIPtr getConfig() const;

	// Miscs
	void enableEchoCancellation(bool enable);
	bool echoCancellationEnabled() const;
	void enableEchoLimiter(bool enable);
	bool echoLimiterEnabled() const;
	void setStaticPictureFps(float fps);
	float getStaticPictureFps() const;
	AddressAPIPtr interpretUrl(StringPtr const &url) const;

	// File
	DECLARE_PROPERTY_FILE(CoreAPI, getRing, setRing);
	DECLARE_PROPERTY_FILE(CoreAPI, getRingback, setRingback);
	DECLARE_PROPERTY_FILE(CoreAPI, getPlayFile, setPlayFile);
	DECLARE_PROPERTY_FILE(CoreAPI, getRecordFile, setRecordFile);
	DECLARE_PROPERTY_FILE(CoreAPI, getRootCa, setRootCa);
	DECLARE_PROPERTY_FILE(CoreAPI, getStaticPicture, setStaticPicture);
	DECLARE_PROPERTY_FILE(CoreAPI, getZrtpSecretsFile, setZrtpSecretsFile);

	// Log
	void setLogHandler(FB::JSObjectPtr const &handler);
	FB::JSObjectPtr getLogHandler() const;

	// uPnP
	bool upnpAvailable() const;
	StringPtr getUpnpExternalIpaddress() const;
	LinphoneUpnpState getUpnpState() const;

	// Tunnel
	bool tunnelAvailable() const;

public: // Event helpers
	FB_JSAPI_EVENT(globalStateChanged, 3, (CoreAPIPtr, const int&, const StringPtr&));
	FB_JSAPI_EVENT(registrationStateChanged, 4, (CoreAPIPtr, ProxyConfigAPIPtr, const int&, const StringPtr&));
	FB_JSAPI_EVENT(callStateChanged, 4, (CoreAPIPtr, CallAPIPtr, const int&, const StringPtr&));
	FB_JSAPI_EVENT(notifyPresenceReceived, 2, (CoreAPIPtr, FriendAPIPtr));
	FB_JSAPI_EVENT(newSubscriptionRequested, 3 ,(CoreAPIPtr, FriendAPIPtr, const StringPtr&));
	FB_JSAPI_EVENT(authInfoRequested, 4, (CoreAPIPtr, const StringPtr&, const StringPtr&, const StringPtr&));
	FB_JSAPI_EVENT(callLogUpdated, 2, (CoreAPIPtr, CallLogAPIPtr));
	FB_JSAPI_EVENT(dtmfReceived, 3, (CoreAPIPtr, CallAPIPtr, const int&));
	FB_JSAPI_EVENT(referReceived, 2, (CoreAPIPtr, const StringPtr&));
	FB_JSAPI_EVENT(callEncryptionChanged, 4, (CoreAPIPtr, CallAPIPtr, const bool&, const StringPtr&));
	FB_JSAPI_EVENT(transferStateChanged, 3, (CoreAPIPtr, CallAPIPtr, const int&));
	FB_JSAPI_EVENT(buddyInfoUpdated, 2, (CoreAPIPtr, FriendAPIPtr));
	FB_JSAPI_EVENT(callStatsUpdated, 3, (CoreAPIPtr, CallAPIPtr, CallStatsAPIPtr));


public: // Internal Use
	inline LinphoneCore *getRef() const {
		return mCore;
	}

	inline void log(std::string const &level, const char *cmsg) {
		if(mLogHandler) {
			mLogHandler->InvokeAsync("", FB::variant_list_of(level)(CHARPTR_TO_STRING(cmsg)));
		}
	}

protected:
	virtual void initProxy();

private:
	mutable FileManagerAPIPtr mFileManager;
	FB::JSObjectPtr mLogHandler;
	StringPtr mMagic;

	void prepareInit();
	void finishInit();
	VideoAPIWeakPtr mVideoWindow;
	static void videoWindowEventHandler(const CoreAPIWeakPtr &corePtr, void *ptr);
	void setVideoWindow(void *ptr);
	VideoAPIWeakPtr mPreviewWindow;
	static void previewWindowEventHandler(const CoreAPIWeakPtr &corePtr, void *ptr);
	void setPreviewWindow(void *ptr);

	LinphoneCore *mCore; // Linphone core object
	LinphoneCoreVTable mVtable;// Linphone callback methods table

	int mIterateInterval;
	bool mIterate;
	FB::TimerPtr mTimer;

	inline void iterate() {
		if (mCore != NULL) {
			linphone_core_iterate(mCore);
		}
	}

	void iterateWithMutex();

protected:
	virtual void onGlobalStateChanged(LinphoneGlobalState gstate, const char *message);
	virtual void onRegistrationStateChanged(LinphoneProxyConfig *cfg, LinphoneRegistrationState cstate, const char *message);
	virtual void onCallStateChanged(LinphoneCall *call, LinphoneCallState cstate, const char *message);
	virtual void onNotifyPresenceReceived(LinphoneFriend * lf);
	virtual void onNewSubscriptionRequested(LinphoneFriend *lf, const char *url);
	virtual void onAuthInfoRequested(const char *realm, const char *username, const char *domain);
	virtual void onCallLogUpdated(LinphoneCallLog *newcl);
	virtual void onMessageReceived(LinphoneChatRoom *room, LinphoneChatMessage *message);
	virtual void onDtmfReceived(LinphoneCall *call, int dtmf);
	virtual void onReferReceived(const char *refer_to);
	virtual void onCallEncryptionChanged(LinphoneCall *call, bool_t on, const char *authentication_token);
	virtual void onTransferStateChanged(LinphoneCall *call, LinphoneCallState state);
	virtual void onBuddyInfoUpdated(LinphoneFriend *lf);
	virtual void onCallStatsUpdated(LinphoneCall *call, const LinphoneCallStats *stats);
	virtual void onInfoReceived(LinphoneCall *call, const LinphoneInfoMessage *info);
	virtual void onSubscriptionStateChanged(LinphoneEvent *event, LinphoneSubscriptionState state);
	virtual void onNotifyReceived(LinphoneEvent *event, const char *notified_event, const LinphoneContent *body);
	virtual void onPublishStateChanged(LinphoneEvent *event, LinphonePublishState state);

private:
	// C Wrappers
	static void wrapper_global_state_changed(LinphoneCore *lc, LinphoneGlobalState gstate, const char *message);
	static void wrapper_registration_state_changed(LinphoneCore *lc, LinphoneProxyConfig *cfg, LinphoneRegistrationState cstate, const char *message);
	static void wrapper_call_state_changed(LinphoneCore *lc, LinphoneCall *call, LinphoneCallState cstate, const char *message);
	static void wrapper_notify_presence_received(LinphoneCore *lc, LinphoneFriend * lf);
	static void wrapper_new_subscription_requested(LinphoneCore *lc, LinphoneFriend *lf, const char *url);
	static void wrapper_auth_info_requested(LinphoneCore *lc, const char *realm, const char *username, const char *domain);
	static void wrapper_call_log_updated(LinphoneCore *lc, LinphoneCallLog *newcl);
	static void wrapper_message_received(LinphoneCore *lc, LinphoneChatRoom *room, LinphoneChatMessage *message);
	static void wrapper_dtmf_received(LinphoneCore *lc, LinphoneCall *call, int dtmf);
	static void wrapper_refer_received(LinphoneCore *lc, const char *refer_to);
	static void wrapper_call_encryption_changed(LinphoneCore *lc, LinphoneCall *call, bool_t on, const char *authentication_token);
	static void wrapper_transfer_state_changed(LinphoneCore *lc, LinphoneCall *call, LinphoneCallState state);
	static void wrapper_buddy_info_updated(LinphoneCore *lc, LinphoneFriend *lf);
	static void wrapper_call_stats_updated(LinphoneCore *lc, LinphoneCall *call, const LinphoneCallStats *stats);
	static void wrapper_info_received(LinphoneCore *lc, LinphoneCall *call, const LinphoneInfoMessage *info);
	static void wrapper_subscription_state_changed(LinphoneCore *lc, LinphoneEvent *event, LinphoneSubscriptionState state);
	static void wrapper_notify_received(LinphoneCore *lc, LinphoneEvent *event, const char *notified_event, const LinphoneContent *body);
	static void wrapper_publish_state_changed(LinphoneCore *lc, LinphoneEvent *event, LinphonePublishState state);
};

} // LinphoneWeb

#endif // H_COREAPI
