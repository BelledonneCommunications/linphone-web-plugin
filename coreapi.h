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
#include <boost/preprocessor/debug/assert.hpp>
#include <boost/mpl/aux_/preprocessor/token_equal.hpp>
#include <JSAPIAuto.h>
#include <BrowserHost.h>
#include <variant_list.h>
#include <SimpleStreamHelper.h>
#include <linphonecore.h>
#include "coreplugin.h"
#include "coreapi.h"
#include "callapi.h"
#include "authinfoapi.h"
#include "payloadtypeapi.h"
#include "proxyconfigapi.h"
#include "utils.h"

#define __DECLARE_SYNC_N_ASYNC_PARAMMACRO(z, n, args) BOOST_PP_ARRAY_ELEM(n, args) p##n
#define __DECLARE_SYNC_N_ASYNC_USEMACRO(z, n, args) p##n

#define REGISTER_SYNC_N_ASYNC(name, funct_name)  \
	registerMethod(name, make_method(this, &CoreAPI::funct_name)); \
	registerMethod(name "_async", make_method(this, &CoreAPI::BOOST_PP_CAT(funct_name, _async)));

#define DECLARE_SYNC_N_ASYNC_SYNC_FCT(name, argCount, argList, ret)  		\
	ret name (BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_PARAMMACRO, (argCount, argList)));

#define BOOST_MPL_PP_TOKEN_EQUAL_void(x) x

#define DECLARE_SYNC_N_ASYNC_THREAD_FCT(name, argCount, argList, ret)  																								\
	BOOST_PP_IF(BOOST_PP_EQUAL(argCount, 0),																\
			void BOOST_PP_CAT(name, _async_thread) (FB::JSObjectPtr callback) {,											\
			void BOOST_PP_CAT(name, _async_thread) (BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_PARAMMACRO, (argCount, argList)), FB::JSObjectPtr callback) {) 	\
	BOOST_PP_IF(BOOST_MPL_PP_TOKEN_EQUAL(ret, void),															\
		name(BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_USEMACRO, (argCount, argList))); 										\
		callback->InvokeAsync("", FB::variant_list_of(shared_from_this()));, 												\
		ret value = name(BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_USEMACRO, (argCount, argList))); 								\
		callback->InvokeAsync("", FB::variant_list_of(shared_from_this())(value));) 											\
		m_threads->remove_thread(boost::this_thread::get_id());														\
	}

#define COMMA ,
#define DECLARE_SYNC_N_ASYNC_ASYNC_FCT(name, argCount, argList) 													\
	BOOST_PP_IF(BOOST_PP_EQUAL(argCount, 0),															\
			void BOOST_PP_CAT(name, _async) (FB::JSObjectPtr callback) {,											\
			void BOOST_PP_CAT(name, _async) (BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_PARAMMACRO, (argCount, argList)), FB::JSObjectPtr callback) {) 	\
		m_threads->create_thread(boost::bind(&CoreAPI::BOOST_PP_CAT(name, _async_thread), this, 						\
		BOOST_PP_IF(BOOST_PP_NOT_EQUAL(argCount, 0), BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_USEMACRO, (argCount, argList)), BOOST_PP_EMPTY())		\
		BOOST_PP_IF(BOOST_PP_NOT_EQUAL(argCount, 0), BOOST_PP_COMMA, BOOST_PP_EMPTY)()										\
													callback)); 							\
																					\
	}

#define DECLARE_SYNC_N_ASYNC(name, argCount, argList, ret)			\
	DECLARE_SYNC_N_ASYNC_SYNC_FCT(name, argCount, argList, ret)		\
	DECLARE_SYNC_N_ASYNC_THREAD_FCT(name, argCount, argList, ret)  		\
	DECLARE_SYNC_N_ASYNC_ASYNC_FCT(name, argCount, argList)

FB_FORWARD_PTR(CoreAPI)
class CoreAPI: public FB::JSAPIAuto {
public:
	CoreAPI(const corePtr& plugin, const FB::BrowserHostPtr& host);
	~CoreAPI();

	corePtr getPlugin();

	// Read-only property
	std::string getVersion();
	std::string getPluginVersion();
	int getSipPort();

	// Property
	const std::string &getMagic();
	void setMagic(const std::string &magic);

	// Call functions
	DECLARE_SYNC_N_ASYNC(invite, 1, (const std::string &), CallAPIPtr);
	void acceptCall(const CallAPIPtr &call);
	void terminateCall(const CallAPIPtr &call);

	// Level functions
	void setPlayLevel(int level);
	int getPlayLevel();
	void setRecLevel(int level);
	int getRecLevel();
	void setRingLevel(int level);
	int getRingLevel();

	// Video functions
	bool videoSupported();
	void enableVideo(bool enable);
	bool videoEnabled();
	void enableVideoPreview(bool enable);
	bool videoPreviewEnabled();
	void setNativeVideoWindowId(unsigned long id);
	unsigned long getNativeVideoWindowId();
	void setNativePreviewWindowId(unsigned long id);
	unsigned long getNativePreviewWindowId();
	bool getUsePreviewWindow();
	void setUsePreviewWindow(bool enable);

	// Sound device functions
	void reloadSoundDevices();
	FB::VariantList getSoundDevices();
	bool soundDeviceCanCapture(const std::string &devid);
	bool soundDeviceCanPlayback(const std::string &devid);
	void setRingerDevice(const std::string &devid);
	void setPlaybackDevice(const std::string &devid);
	void setCaptureDevice(const std::string &devid);
	std::string getRingerDevice();
	std::string getPlaybackDevice();
	std::string getCaptureDevice();

	// Video device functions
	void reloadVideoDevices();
	FB::VariantList getVideoDevices();
	void setVideoDevice(const std::string &devid);
	std::string getVideoDevice();

	// Codecs functions
	FB::VariantList getAudioCodecs();
	FB::VariantList getVideoCodecs();
	void setAudioCodecs(const std::vector<FB::JSAPIPtr> &list);
	void setVideoCodecs(const std::vector<FB::JSAPIPtr> &list);

	// ProxyConfig functions
	int addProxyConfig(const ProxyConfigAPIPtr &config);
	void clearProxyConfig();
	void removeProxyConfig(const ProxyConfigAPIPtr &config);
	FB::VariantList getProxyConfigList();
	void setDefaultProxy(const ProxyConfigAPIPtr &config);
	ProxyConfigAPIPtr getDefaultProxy();

	// AuthInfo functions
	void addAuthInfo(const AuthInfoAPIPtr &authInfo);

	// Instantiator functions
	ProxyConfigAPIPtr newProxyConfig();
	AuthInfoAPIPtr newAuthInfo(const std::string &username, const std::string &userid,
			const std::string &passwd, const std::string &ha1, const std::string &realm);

	// Dtmf
	void sendDtmf(const std::string &dtmf);
	void stopDtmf();
	void playDtmf(const std::string &dtmf, int duration_ms);

	// Miscs
	int init();
	void enableEchoCancellation(bool enable);
	bool echoCancellationEnabled();
	void enableEchoLimiter(bool enable);
	bool echoLimiterEnabled();
	void enableIpv6(bool enable);
	bool ipv6Enabled();
	void enableKeepAlive(bool enable);
	bool keepAliveEnabled();

	// File
	std::string getRing();
	void setRing(const std::string &ring);
	void setRingAsync(const std::string &ring, const FB::JSObjectPtr& callback);
private:
	void setRingCallback(const std::string& download, const std::string& file, const FB::JSObjectPtr& callback);
public:

	// Download
	typedef boost::function<void (const std::string&, const std::string&)> DownloadCallbackType;
	void download(const std::string& url, const DownloadCallbackType& functionback);
	void downloadCallback(const std::string& url, bool success, const FB::HeaderMap& headers,
			const boost::shared_array<uint8_t>& data, const size_t size, const DownloadCallbackType& functionback);

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

	boost::mutex m_core_mutex;
	boost::thread *m_core_thread;
	mythread_group *m_threads;

	std::string m_internal_ring;

	void initProxy();

	void iterate() {
		if (mCore != NULL)
			linphone_core_iterate(mCore);
	}

	friend void linphone_iterate_thread(CoreAPI *linphone_api);
	void iterateWithMutex() {
		boost::mutex::scoped_lock scopedLock(m_core_mutex);
		iterate();
	}

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
