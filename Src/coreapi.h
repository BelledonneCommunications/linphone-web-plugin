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
FB_FORWARD_PTR(CallStatsParamsAPI)
FB_FORWARD_PTR(CoreAPI)
FB_FORWARD_PTR(PayloadTypeAPI)
FB_FORWARD_PTR(PointerAPI)
FB_FORWARD_PTR(ProxyConfigAPI)
FB_FORWARD_PTR(SipTransportsAPI)
FB_FORWARD_PTR(VideoPolicyAPI)
	
FB_FORWARD_PTR(VideoAPI)

FB_FORWARD_PTR(FileManagerAPI)

FB_FORWARD_PTR(CoreAPI)
class CoreAPI: public WrapperAPI {
	friend class FactoryAPI;
public:
	CoreAPI();
	virtual ~CoreAPI();

	// Read-only property
	StringPtr getVersion() const;
	StringPtr getPluginVersion() const;

	// Property
	StringPtr const &getMagic() const;
	void setMagic(StringPtr const &magic);

	// Call functions
	DECLARE_SYNC_N_ASYNC(CoreAPI, invite, 1, (StringPtr const &), CallAPIPtr);
	DECLARE_SYNC_N_ASYNC(CoreAPI, inviteAddress, 1, (AddressAPIPtr const &), CallAPIPtr);
	DECLARE_SYNC_N_ASYNC(CoreAPI, inviteWithParams, 2, (StringPtr const &, CallParamsAPIPtr const &), CallAPIPtr);
	DECLARE_SYNC_N_ASYNC(CoreAPI, inviteAddressWithParams, 2, (AddressAPIPtr const &, CallParamsAPIPtr const &), CallAPIPtr);
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
	void muteMic(bool muted);
	bool isMicMuted() const;
	float getMicGainDb() const;
	void setMicGainDb(float gain);
	float getPlaybackGainDb() const;
	void setPlaybackGainDb(float gain);

	// Video functions
	bool videoSupported() const;
	void enableVideo(bool enable);
	bool videoEnabled() const;
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
	void setDownloadPtime(int ptime);
	int getDownloadPtime() const;
	void setUploadPtime(int ptime);
	int getUploadPtime() const;
	void setMtu(int mtu);
	int getMtu() const;
	void setStunServer(StringPtr const &server);
	StringPtr getStunServer() const;
	void setRelayAddr(StringPtr const &addr);
	StringPtr getRelayAddr() const;
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
	AuthInfoAPIPtr findAuthInfo(StringPtr const &realm, StringPtr const &username);
	std::vector<AuthInfoAPIPtr> getAuthInfoList() const;
	void clearAllAuthInfo();

	// Instantiator functions
	FileManagerAPIPtr getFileManager() const;
	ProxyConfigAPIPtr newProxyConfig() const;
	AuthInfoAPIPtr newAuthInfo(StringPtr const &username, StringPtr const &userid,
			StringPtr const &passwd, StringPtr const &ha1, StringPtr const &realm) const;
	AddressAPIPtr newAddress(StringPtr const &address) const;

	// Dtmf
	void sendDtmf(std::string const &dtmf);
	void stopDtmf();
	void playDtmf(std::string const &dtmf, int duration_ms);
	bool getUseInfoForDtmf() const;
	void setUseInfoForDtmf(bool enable);
	bool getUseRfc2833ForDtmf() const;
	void setUseRfc2833ForDtmf(bool enable);

	// Presence
	int getPresenceInfo() const;
	void setPresenceInfo(int status);
	
	// Core helpers
	int init(StringPtr const &config, StringPtr const &factory);
	int uninit();
	void setIterateInterval(int ms);
	int getIterateInterval() const;
	void enableIterate(bool enable);
	bool iterateEnabled() const;
	
	// Miscs
	void enableEchoCancellation(bool enable);
	bool echoCancellationEnabled() const;
	void enableEchoLimiter(bool enable);
	bool echoLimiterEnabled() const;
	void setStaticPictureFps(float fps);
	float getStaticPictureFps() const;
	StringPtr getUserAgentName() const;
	void setUserAgentName(StringPtr const &name);
	StringPtr getUserAgentVersion() const;
	void setUserAgentVersion(StringPtr const &version);
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
	FB_JSAPI_EVENT(callStateChanged, 4, (CoreAPIPtr, CallAPIPtr, const int&, const StringPtr&));
	FB_JSAPI_EVENT(registrationStateChanged, 4, (CoreAPIPtr, ProxyConfigAPIPtr, const int&, const StringPtr&));
	FB_JSAPI_EVENT(authInfoRequested, 3, (CoreAPIPtr, const StringPtr&, const StringPtr&));
	FB_JSAPI_EVENT(referReceived, 2, (CoreAPIPtr, const StringPtr&));
	FB_JSAPI_EVENT(displayStatus, 2, (CoreAPIPtr, const StringPtr&));
	FB_JSAPI_EVENT(displayMessage, 2, (CoreAPIPtr, const StringPtr&));
	FB_JSAPI_EVENT(displayWarning, 2, (CoreAPIPtr, const StringPtr&));
	FB_JSAPI_EVENT(displayUrl, 3, (CoreAPIPtr, const StringPtr&, const StringPtr&));
	FB_JSAPI_EVENT(show, 1, (CoreAPIPtr));


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
#ifdef CORE_THREADED
	boost::shared_ptr<boost::thread> mCoreThread;
#else
	FB::TimerPtr mTimer;
#endif //CORE_THREADED

	inline void iterate() {
		if (mCore != NULL) {
			linphone_core_iterate(mCore);
		}
	}

#ifdef CORE_THREADED
	static void* refLib();
	static void unrefLib(void *libHandle);
	static void destroyThread(LinphoneCore *core, void *libHandle);
	static void iterateThread(CoreAPIPtr &core);
	void iterateWithMutex();
#endif

protected:
	virtual void onGlobalStateChanged(LinphoneGlobalState gstate, const char *message);
	virtual void onRegistrationStateChanged(LinphoneProxyConfig *cfg, LinphoneRegistrationState cstate, const char *message);
	virtual void onCallStateChanged(LinphoneCall *call, LinphoneCallState cstate, const char *message);
	virtual void onNotifyPresenceRecv(LinphoneFriend * lf);
	virtual void onNewSubscriptionRequest(LinphoneFriend *lf, const char *url);
	virtual void onAuthInfoRequested(const char *realm, const char *username);
	virtual void onCallLogUpdated(LinphoneCallLog *newcl);
	virtual void onTextReceived(LinphoneChatRoom *room, const LinphoneAddress *from, const char *message);
	virtual void onDtmfReceived(LinphoneCall *call, int dtmf);
	virtual void onReferReceived(const char *refer_to);
	virtual void onBuddyInfoUpdated(LinphoneFriend *lf);
	//virtual void onNotifyRecv(LinphoneCall *call, const char *from, const char *event);
	virtual void onDisplayStatus(const char *message);
	virtual void onDisplayMessage(const char *message);
	virtual void onDisplayWarning(const char *message);
	virtual void onDisplayUrl(const char *message, const char *url);
	virtual void onShow();
	virtual void onCallEncryptionChanged(LinphoneCall *call, bool_t on, const char *authentication_token);

private:
	// C Wrappers
	static void wrapper_global_state_changed(LinphoneCore *lc, LinphoneGlobalState gstate, const char *message);
	static void wrapper_registration_state_changed(LinphoneCore *lc, LinphoneProxyConfig *cfg, LinphoneRegistrationState cstate, const char *message);
	static void wrapper_call_state_changed(LinphoneCore *lc, LinphoneCall *call, LinphoneCallState cstate, const char *message);
	static void wrapper_notify_presence_recv(LinphoneCore *lc, LinphoneFriend * lf);
	static void wrapper_new_subscription_request(LinphoneCore *lc, LinphoneFriend *lf, const char *url);
	static void wrapper_auth_info_requested(LinphoneCore *lc, const char *realm, const char *username);
	static void wrapper_call_log_updated(LinphoneCore *lc, LinphoneCallLog *newcl);
	static void wrapper_text_received(LinphoneCore *lc, LinphoneChatRoom *room, const LinphoneAddress *from, const char *message);
	static void wrapper_dtmf_received(LinphoneCore *lc, LinphoneCall *call, int dtmf);
	static void wrapper_refer_received(LinphoneCore *lc, const char *refer_to);
	static void wrapper_buddy_info_updated(LinphoneCore *lc, LinphoneFriend *lf);
	//static void wrapper_notify_recv(LinphoneCore *lc, LinphoneCall *call, const char *from, const char *event);
	static void wrapper_display_status(LinphoneCore *lc, const char *message);
	static void wrapper_display_message(LinphoneCore *lc, const char *message);
	static void wrapper_display_warning(LinphoneCore *lc, const char *message);
	static void wrapper_display_url(LinphoneCore *lc, const char *message, const char *url);
	static void wrapper_show(LinphoneCore *lc);
	static void wrapper_call_encryption_changed(LinphoneCore *lc, LinphoneCall *call, bool_t on, const char *authentication_token);
};
	
} // LinphoneWeb

#endif // H_COREAPI
