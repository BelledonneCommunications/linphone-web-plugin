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
#include <boost/weak_ptr.hpp>
#include <JSAPIAuto.h>
#include <BrowserHost.h>
#include <Timer.h>
#include <variant_list.h>
#include "SimpleStreamHelper.h"
#include <linphonecore.h>
#include "addressapi.h"
#include "coreplugin.h"
#include "coreapi.h"
#include "callparamsapi.h"
#include "callapi.h"
#include "authinfoapi.h"
#include "payloadtypeapi.h"
#include "proxyconfigapi.h"
#include "utils.h"
#include "macro.h"

FB_FORWARD_PTR(CoreAPI)
class CoreAPI: public FB::JSAPIAuto {
public:
	CoreAPI(const corePtr& plugin, const FB::BrowserHostPtr& host);
	~CoreAPI();

	corePtr getPlugin();

	// Read-only property
	std::string getVersion() const;
	std::string getPluginVersion() const;

	// Property
	const std::string &getMagic() const;
	void setMagic(const std::string &magic);

	// Call functions
	DECLARE_SYNC_N_ASYNC(CoreAPI, invite, 1, (const std::string &), CallAPIPtr);
	DECLARE_SYNC_N_ASYNC(CoreAPI, inviteAddress, 1, (const AddressAPIPtr &), CallAPIPtr);
	DECLARE_SYNC_N_ASYNC(CoreAPI, inviteWithParams, 2, (const std::string &, const CallParamsAPIPtr &), CallAPIPtr);
	DECLARE_SYNC_N_ASYNC(CoreAPI, inviteAddressWithParams, 2, (const AddressAPIPtr &, const CallParamsAPIPtr &), CallAPIPtr);
	int acceptCall(const CallAPIPtr &call);
	int acceptCallWithParams(const CallAPIPtr &call, const CallParamsAPIPtr &params);
	CallAPIPtr getCurrentCall();
	int terminateCall(const CallAPIPtr &call);
	int terminateAllCalls();
	int redirectCall(const CallAPIPtr &call, const std::string &uri);
	int declineCall(const CallAPIPtr &call, int reason);
	int transferCall(const CallAPIPtr &call, const std::string &uri);
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

	// Sound device functions
	void reloadSoundDevices();
	FB::VariantList getSoundDevices() const;
	bool soundDeviceCanCapture(const std::string &devid) const;
	bool soundDeviceCanPlayback(const std::string &devid) const;
	void setRingerDevice(const std::string &devid);
	void setPlaybackDevice(const std::string &devid);
	void setCaptureDevice(const std::string &devid);
	std::string getRingerDevice() const;
	std::string getPlaybackDevice() const;
	std::string getCaptureDevice() const;

	// Video device functions
	void reloadVideoDevices();
	FB::VariantList getVideoDevices() const;
	void setVideoDevice(const std::string &devid);
	std::string getVideoDevice() const;

	// Codecs functions
	FB::VariantList getAudioCodecs() const;
	FB::VariantList getVideoCodecs() const;
	void setAudioCodecs(const std::vector<FB::JSAPIPtr> &list);
	void setVideoCodecs(const std::vector<FB::JSAPIPtr> &list);
	bool payloadTypeEnabled(const PayloadTypeAPIPtr &payloadType) const;
	void enablePayloadType(const PayloadTypeAPIPtr &payloadType, bool enable);

	// ProxyConfig functions
	int addProxyConfig(const ProxyConfigAPIPtr &config);
	void clearProxyConfig();
	void removeProxyConfig(const ProxyConfigAPIPtr &config);
	FB::VariantList getProxyConfigList() const;
	void setDefaultProxy(const ProxyConfigAPIPtr &config);
	ProxyConfigAPIPtr getDefaultProxy() const;
	void setPrimaryContact(const std::string &contact);
	std::string getPrimaryContact() const;
	void refreshRegisters();

	// Network functions
	void setAudioPort(int port);
	int getAudioPort() const;
	void setVideoPort(int port);
	int getVideoPort() const;
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
	void setStunServer(const std::string &server);
	std::string getStunServer() const;
	void setRelayAddr(const std::string &addr);
	std::string getRelayAddr() const;
	void setNatAddress(const std::string &address);
	std::string getNatAddress() const;
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
	bool adaptiveRateControlEnabled() const;
	void enableAdaptiveRateControl(bool enable);
	bool isNetworkReachable() const;
	void setNetworkReachable(bool reachable);

	// AuthInfo functions
	void addAuthInfo(const AuthInfoAPIPtr &authInfo);
	void abortAuthentication(const AuthInfoAPIPtr &authInfo);
	void removeAuthInfo(const AuthInfoAPIPtr &authInfo);
	AuthInfoAPIPtr findAuthInfo(const std::string &realm, const std::string &username);
	FB::VariantList getAuthInfoList() const;
	void clearAllAuthInfo();

	// Instantiator functions
	ProxyConfigAPIPtr newProxyConfig();
	AuthInfoAPIPtr newAuthInfo(const std::string &username, const std::string &userid,
			const std::string &passwd, const std::string &ha1, const std::string &realm);

	// Dtmf
	void sendDtmf(const std::string &dtmf);
	void stopDtmf();
	void playDtmf(const std::string &dtmf, int duration_ms);
	bool getUseInfoForDtmf() const;
	void setUseInfoForDtmf(bool enable);
	bool getUseRfc2833ForDtmf() const;
	void setUseRfc2833ForDtmf(bool enable);

	// Miscs
	int init();
	void enableEchoCancellation(bool enable);
	bool echoCancellationEnabled() const;
	void enableEchoLimiter(bool enable);
	bool echoLimiterEnabled() const;
	void setStaticPictureFps(float fps);
	float getStaticPictureFps() const;

	// File
	DECLARE_PROPERTY_FILE(CoreAPI, getRing, setRing);
	DECLARE_PROPERTY_FILE(CoreAPI, getRingback, setRingback);
	DECLARE_PROPERTY_FILE(CoreAPI, getRootCa, setRootCa);
	DECLARE_PROPERTY_FILE(CoreAPI, getStaticPicture, setStaticPicture);
	DECLARE_PROPERTY_FILE(CoreAPI, getZrtpSecretsFile, setZrtpSecretsFile);

	// Download
	void download(const std::string& url, const FB::JSObjectPtr& callback);
private:
	void downloadCallback(const FB::URI& url, bool success, const FB::HeaderMap& headers,
			const boost::shared_array<uint8_t>& data, const size_t size, const FB::JSObjectPtr& callback);
	void downloadProgressCallback(const FB::URI& url, const size_t received, const size_t total, const FB::JSObjectPtr& callback);
public:

	// Event helpers
	FB_JSAPI_EVENT(globalStateChanged, 3, (CoreAPIPtr, const int&, const std::string&));
	FB_JSAPI_EVENT(callStateChanged, 4, (CoreAPIPtr, CallAPIPtr, const int&, const std::string&));
	FB_JSAPI_EVENT(registrationStateChanged, 4, (CoreAPIPtr, ProxyConfigAPIPtr, const int&, const std::string&));
	FB_JSAPI_EVENT(authInfoRequested, 3, (CoreAPIPtr, const std::string&, const std::string&));
	FB_JSAPI_EVENT(referReceived, 2, (CoreAPIPtr, const std::string&));
	FB_JSAPI_EVENT(displayStatus, 2, (CoreAPIPtr, const std::string&));
	FB_JSAPI_EVENT(displayMessage, 2, (CoreAPIPtr, const std::string&));
	FB_JSAPI_EVENT(displayWarning, 2, (CoreAPIPtr, const std::string&));
	FB_JSAPI_EVENT(displayUrl, 3, (CoreAPIPtr, const std::string&, const std::string&));
	FB_JSAPI_EVENT(show, 1, (CoreAPIPtr));

	inline LinphoneCore *getRef() const {
		return mCore;
	}

	static CoreAPIPtr get(LinphoneCore *core);
private :
	std::string m_magic;
	coreWeakPtr m_plugin;
	FB::BrowserHostPtr m_host;

	LinphoneCore *mCore; // Linphone core object
	LinphoneCoreVTable mVtable;// Linphone callback methods table

#ifdef CORE_THREADED
	mutable boost::mutex m_core_mutex;
	boost::thread *m_core_thread;
	mythread_group *m_threads;
#else
	FB::TimerPtr m_timer;
#endif //CORE_THREADED

	void initProxy();

	void iterate() {
		if (mCore != NULL)
			linphone_core_iterate(mCore);
	}

#ifdef CORE_THREADED
	friend void linphone_iterate_thread(CoreAPI *linphone_api);
	void iterateWithMutex() {
		boost::mutex::scoped_lock scopedLock(m_core_mutex);
		iterate();
	}
#endif

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

#endif // H_COREAPI
