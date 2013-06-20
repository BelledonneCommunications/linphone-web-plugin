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

#ifndef H_COREAPI
#define H_COREAPI

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <Timer.h>
#include <variant_list.h>

#include <linphonecore.h>
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
FB_FORWARD_PTR(ProxyConfigAPI)
FB_FORWARD_PTR(SipTransportsAPI)
FB_FORWARD_PTR(VideoPolicyAPI)

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
	const StringPtr &getMagic() const;
	void setMagic(const StringPtr &magic);

	// Call functions
	DECLARE_SYNC_N_ASYNC(CoreAPI, invite, 1, (const StringPtr &), CallAPIPtr);
	DECLARE_SYNC_N_ASYNC(CoreAPI, inviteAddress, 1, (const AddressAPIPtr &), CallAPIPtr);
	DECLARE_SYNC_N_ASYNC(CoreAPI, inviteWithParams, 2, (const StringPtr &, const CallParamsAPIPtr &), CallAPIPtr);
	DECLARE_SYNC_N_ASYNC(CoreAPI, inviteAddressWithParams, 2, (const AddressAPIPtr &, const CallParamsAPIPtr &), CallAPIPtr);
	int acceptCall(const CallAPIPtr &call);
	int acceptCallWithParams(const CallAPIPtr &call, const CallParamsAPIPtr &params);
	CallAPIPtr getCurrentCall();
	int terminateCall(const CallAPIPtr &call);
	int terminateAllCalls();
	int redirectCall(const CallAPIPtr &call, const StringPtr &uri);
	int declineCall(const CallAPIPtr &call, int reason);
	int transferCall(const CallAPIPtr &call, const StringPtr &uri);
	int transferCallToAnother(const CallAPIPtr &call, const CallAPIPtr &dest);
	int resumeCall(const CallAPIPtr &call);
	int pauseCall(const CallAPIPtr &call);
	int pauseAllCalls();
	int updateCall(const CallAPIPtr &call, const CallParamsAPIPtr &params);
	int deferCallUpdate(const CallAPIPtr &call);
	int acceptCallUpdate(const CallAPIPtr &call, const CallParamsAPIPtr &params);
	CallParamsAPIPtr createDefaultCallParameters();
	int getIncTimeout() const;
	void setIncTimeout(int timeout);
	int getInCallTimeout() const;
	void setInCallTimeout(int timeout);
	int getMaxCalls() const;
	void setMaxCalls(int max);
	std::vector<CallAPIPtr> getCalls() const;
	int getCallsNb() const;
	int getMissedCallsCount() const;

	// Conference functions
	int addAllToConference();
	int addToConference(const CallAPIPtr &call);
	int enterConference();
	int getConferenceSize() const;
	bool isInConference() const;
	int leaveConference();
	int removeFromConference(const CallAPIPtr &call);
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
	void setNativeVideoWindowId(unsigned long id);
	unsigned long getNativeVideoWindowId() const;
	void setNativePreviewWindowId(unsigned long id);
	unsigned long getNativePreviewWindowId() const;
	bool getUsePreviewWindow() const;
	void setUsePreviewWindow(bool enable);
	VideoPolicyAPIPtr getVideoPolicy() const;
	void setVideoPolicy(const VideoPolicyAPIPtr &videoPolicy);

	// Sound device functions
	void reloadSoundDevices();
	std::vector<StringPtr> getSoundDevices() const;
	bool soundDeviceCanCapture(const StringPtr &devid) const;
	bool soundDeviceCanPlayback(const StringPtr &devid) const;
	void setRingerDevice(const StringPtr &devid);
	void setPlaybackDevice(const StringPtr &devid);
	void setCaptureDevice(const StringPtr &devid);
	StringPtr getRingerDevice() const;
	StringPtr getPlaybackDevice() const;
	StringPtr getCaptureDevice() const;

	// Video device functions
	void reloadVideoDevices();
	std::vector<StringPtr> getVideoDevices() const;
	void setVideoDevice(const StringPtr &devid);
	StringPtr getVideoDevice() const;

	// Codecs functions
	std::vector<PayloadTypeAPIPtr> getAudioCodecs() const;
	std::vector<PayloadTypeAPIPtr> getVideoCodecs() const;
	void setAudioCodecs(const std::vector<PayloadTypeAPIPtr> &list);
	void setVideoCodecs(const std::vector<PayloadTypeAPIPtr> &list);
	bool payloadTypeEnabled(const PayloadTypeAPIPtr &payloadType) const;
	void enablePayloadType(const PayloadTypeAPIPtr &payloadType, bool enable);

	// ProxyConfig functions
	int addProxyConfig(const ProxyConfigAPIPtr &config);
	void clearProxyConfig();
	void removeProxyConfig(const ProxyConfigAPIPtr &config);
	std::vector<ProxyConfigAPIPtr> getProxyConfigList() const;
	void setDefaultProxy(const ProxyConfigAPIPtr &config);
	ProxyConfigAPIPtr getDefaultProxy() const;
	void setPrimaryContact(const StringPtr &contact);
	StringPtr getPrimaryContact() const;
	void refreshRegisters();
	
	// CallLog functions
	void clearCallLogs();
	std::vector<CallLogAPIPtr> getCallLogs() const;

	// Network functions
	void setAudioPort(int port);
	int getAudioPort() const;
	void setAudioPortRange(const StringPtr &range);
	StringPtr getAudioPortRange() const;
	void setVideoPort(int port);
	int getVideoPort() const;
	void setVideoPortRange(const StringPtr &range);
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
	void setStunServer(const StringPtr &server);
	StringPtr getStunServer() const;
	void setRelayAddr(const StringPtr &addr);
	StringPtr getRelayAddr() const;
	void setNatAddress(const StringPtr &address);
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
	void setSipTransports(const SipTransportsAPIPtr &sipTransports);
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

	// AuthInfo functions
	void addAuthInfo(const AuthInfoAPIPtr &authInfo);
	void abortAuthentication(const AuthInfoAPIPtr &authInfo);
	void removeAuthInfo(const AuthInfoAPIPtr &authInfo);
	AuthInfoAPIPtr findAuthInfo(const StringPtr &realm, const StringPtr &username);
	std::vector<AuthInfoAPIPtr> getAuthInfoList() const;
	void clearAllAuthInfo();

	// Instantiator functions
	FileManagerAPIPtr getFileManager() const;
	ProxyConfigAPIPtr newProxyConfig() const;
	AuthInfoAPIPtr newAuthInfo(const StringPtr &username, const StringPtr &userid,
			const StringPtr &passwd, const StringPtr &ha1, const StringPtr &realm) const;

	// Dtmf
	void sendDtmf(const std::string &dtmf);
	void stopDtmf();
	void playDtmf(const std::string &dtmf, int duration_ms);
	bool getUseInfoForDtmf() const;
	void setUseInfoForDtmf(bool enable);
	bool getUseRfc2833ForDtmf() const;
	void setUseRfc2833ForDtmf(bool enable);


	// Core helpers
	int init(const StringPtr &config, const StringPtr &factory);
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
	void setUserAgentName(const StringPtr &name);
	StringPtr getUserAgentVersion() const;
	void setUserAgentVersion(const StringPtr &version);

	// File
	DECLARE_PROPERTY_FILE(CoreAPI, getRing, setRing);
	DECLARE_PROPERTY_FILE(CoreAPI, getRingback, setRingback);
	DECLARE_PROPERTY_FILE(CoreAPI, getPlayFile, setPlayFile);
	DECLARE_PROPERTY_FILE(CoreAPI, getRecordFile, setRecordFile);
	DECLARE_PROPERTY_FILE(CoreAPI, getRootCa, setRootCa);
	DECLARE_PROPERTY_FILE(CoreAPI, getStaticPicture, setStaticPicture);
	DECLARE_PROPERTY_FILE(CoreAPI, getZrtpSecretsFile, setZrtpSecretsFile);
	
	// Log
	void setLogHandler(const FB::JSObjectPtr &handler);
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
	
	inline void log(const std::string &level, const StringPtr &msg) {
		if(mLogHandler) {
			mLogHandler->InvokeAsync("", FB::variant_list_of(level)(msg));
		}
	}
	

protected:
	virtual void initProxy();
	
private:
	mutable FileManagerAPIPtr mFileManager;
	FB::JSObjectPtr mLogHandler;
	StringPtr mMagic;

	LinphoneCore *mCore; // Linphone core object
	LinphoneCoreVTable mVtable;// Linphone callback methods table

	int mIterateInterval;
#ifdef CORE_THREADED
	boost::shared_ptr<boost::thread> mCoreThread;
	bool mIterate;
#else
	FB::TimerPtr mTimer;
#endif //CORE_THREADED

	inline void iterate() {
		if (mCore != NULL) {
			linphone_core_iterate(mCore);
		}
	}

#ifdef CORE_THREADED
	static void *libHandle;
	static void refLib();
	static void unrefLib();
	static void destroyThread(LinphoneCore *core);
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
	virtual void onNotifyRecv(LinphoneCall *call, const char *from, const char *event);
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
	static void wrapper_notify_recv(LinphoneCore *lc, LinphoneCall *call, const char *from, const char *event);
	static void wrapper_display_status(LinphoneCore *lc, const char *message);
	static void wrapper_display_message(LinphoneCore *lc, const char *message);
	static void wrapper_display_warning(LinphoneCore *lc, const char *message);
	static void wrapper_display_url(LinphoneCore *lc, const char *message, const char *url);
	static void wrapper_show(LinphoneCore *lc);
	static void wrapper_call_encryption_changed(LinphoneCore *lc, LinphoneCall *call, bool_t on, const char *authentication_token);
};
	
} // LinphoneWeb

#endif // H_COREAPI
