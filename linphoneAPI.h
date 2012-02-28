/**********************************************************
 Auto-generated linphoneAPI.h

 \**********************************************************/

#include <string>
#include <sstream>
#include <vector>
#include <boost/weak_ptr.hpp>
#include "JSAPIAuto.h"
#include "BrowserHost.h"
#include "linphone.h"
#include "LinphoneCallAPI.h"
#include <linphonecore.h>
#ifndef H_linphoneAPI
#define H_linphoneAPI


class linphoneAPI: public FB::JSAPIAuto {
public:
	linphoneAPI(const linphonePtr& plugin, const FB::BrowserHostPtr& host);
	int init();
	void shutdown();
	~linphoneAPI();

	linphonePtr getPlugin();

	// Read-only property ${PROPERTY.ident}
	std::string getVersion();

	boost::shared_ptr<LinphoneCallAPI> invite(const std::string &dest);
	void terminate_call(boost::shared_ptr<LinphoneCallAPI> call);
	void set_play_level(int level);
	void set_rec_level(int level);

	// Event helpers
	FB_JSAPI_EVENT(global_state_changed, 2, (const int&, const std::string&))
	FB_JSAPI_EVENT(call_state_changed, 3, (boost::shared_ptr<LinphoneCallAPI>, const int&, const std::string&))

	FB_JSAPI_EVENT(auth_info_requested, 2, (const std::string&, const std::string&))

	FB_JSAPI_EVENT(refer_received, 1, (const std::string&))

	FB_JSAPI_EVENT(display_status, 1, (const std::string&))
	FB_JSAPI_EVENT(display_message, 1, (const std::string&))
	FB_JSAPI_EVENT(display_warning, 1, (const std::string&))
	FB_JSAPI_EVENT(display_url, 2, (const std::string&, const std::string&))
	FB_JSAPI_EVENT(show, 0, ())

private:
	linphoneWeakPtr m_plugin;
	FB::BrowserHostPtr m_host;

	LinphoneCore *m_lin_core; // Linphone core object
	LinphoneCoreVTable m_lin_vtable; // Linphone callback methods table
	boost::mutex m_core_mutex;
	boost::thread *m_core_thread;

	void iterate() {
		if (m_lin_core != NULL)
			linphone_core_iterate(m_lin_core);
	}

	friend void linphone_thread (linphoneAPI *linphone_api);
	void iterateWithMutex() {
		//m_core_mutex.lock();
		iterate();
		//m_core_mutex.unlock();
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

#endif // H_linphoneAPI
