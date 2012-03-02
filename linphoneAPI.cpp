#include <JSObject.h>
#include <variant_list.h>
#include <DOM/Document.h>
#include <global/config.h>
#include <sstream>
#include <boost/foreach.hpp>
#include "linphoneAPI.h"

void linphone_iterate_thread(linphoneAPI *linphone_api) {
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

	FBLOG_DEBUG("linphone_destroy_thread", "end");
}

///////////////////////////////////////////////////////////////////////////////
/// @fn linphoneAPI::linphoneAPI(const linphonePtr& plugin, const FB::BrowserHostPtr host)
///
/// @brief  Constructor for your JSAPI object.  You should register your methods, properties, and events
///         that should be accessible to Javascript from here.
///
/// @see FB::JSAPIAuto::registerMethod
/// @see FB::JSAPIAuto::registerProperty
/// @see FB::JSAPIAuto::registerEvent
///////////////////////////////////////////////////////////////////////////////
linphoneAPI::linphoneAPI(const linphonePtr& plugin, const FB::BrowserHostPtr& host) :
		m_plugin(plugin), m_host(host), m_lin_core(NULL), m_core_thread(NULL), m_threads(new mythread_group()) {
	FBLOG_DEBUG("linphoneAPI::linphoneAPI()", this);

	// Read-only property
	registerProperty("version", make_property(this, &linphoneAPI::getVersion));
	registerProperty("sip_port", make_property(this, &linphoneAPI::getSipPort));

	// Methods
	registerMethod("init", make_method(this, &linphoneAPI::init));
	REGISTER_SYNC_N_ASYNC("invite", invite);
	REGISTER_SYNC_N_ASYNC("terminate_call", terminate_call);
	registerMethod("set_play_level", make_method(this, &linphoneAPI::set_play_level));
	registerMethod("set_rec_level", make_method(this, &linphoneAPI::set_rec_level));
}

int linphoneAPI::init() {
	srand(time(NULL));

	// Disable logs
	linphone_core_disable_logs();

	FBLOG_DEBUG("linphoneAPI::init()", this);

	// Initialize callback table
	memset(&m_lin_vtable, 0, sizeof(LinphoneCoreVTable));
	m_lin_vtable.global_state_changed = linphoneAPI::wrapper_global_state_changed;
	m_lin_vtable.registration_state_changed = linphoneAPI::wrapper_registration_state_changed;
	m_lin_vtable.call_state_changed = linphoneAPI::wrapper_call_state_changed;
	m_lin_vtable.notify_presence_recv = linphoneAPI::wrapper_notify_presence_recv;
	m_lin_vtable.new_subscription_request = linphoneAPI::wrapper_new_subscription_request;
	m_lin_vtable.auth_info_requested = linphoneAPI::wrapper_auth_info_requested;
	m_lin_vtable.call_log_updated = linphoneAPI::wrapper_call_log_updated;
	m_lin_vtable.text_received = linphoneAPI::wrapper_text_received;
	m_lin_vtable.dtmf_received = linphoneAPI::wrapper_dtmf_received;
	m_lin_vtable.refer_received = linphoneAPI::wrapper_refer_received;
	m_lin_vtable.buddy_info_updated = linphoneAPI::wrapper_buddy_info_updated;
	m_lin_vtable.notify_recv = linphoneAPI::wrapper_notify_recv;
	m_lin_vtable.display_status = linphoneAPI::wrapper_display_status;
	m_lin_vtable.display_message = linphoneAPI::wrapper_display_message;
	m_lin_vtable.display_warning = linphoneAPI::wrapper_display_warning;
	m_lin_vtable.display_url = linphoneAPI::wrapper_display_url;
	m_lin_vtable.show = linphoneAPI::wrapper_show;
	m_lin_vtable.call_encryption_changed = linphoneAPI::wrapper_call_encryption_changed;

	m_lin_core = linphone_core_new(&m_lin_vtable, NULL, NULL, (void *) this);
	if (linphone_core_get_user_data(m_lin_core) != this) {
		FBLOG_ERROR("linphoneAPI::init()", "Too old version of linphone core!");
		return 1;
	}

	int port = 5000 + rand() % 5000;
	FBLOG_DEBUG("linphoneAPI::init()", port);
	linphone_core_set_sip_port(m_lin_core, port);

	m_core_thread = new boost::thread(linphone_iterate_thread, this);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
/// @fn linphoneAPI::~linphoneAPI()
///
/// @brief  Destructor.  Remember that this object will not be released until
///         the browser is done with it; this will almost definitely be after
///         the plugin is released.
///////////////////////////////////////////////////////////////////////////////
linphoneAPI::~linphoneAPI() {
	FBLOG_DEBUG("linphoneAPI::~linphoneAPI()", this);

	linphone_core_set_user_data(m_lin_core, NULL);

	boost::thread t(linphone_destroy_thread, m_lin_core, m_core_thread, m_threads);
}

///////////////////////////////////////////////////////////////////////////////
/// @fn linphonePtr linphoneAPI::getPlugin()
///
/// @brief  Gets a reference to the plugin that was passed in when the object
///         was created.  If the plugin has already been released then this
///         will throw a FB::script_error that will be translated into a
///         javascript exception in the page.
///////////////////////////////////////////////////////////////////////////////
linphonePtr linphoneAPI::getPlugin() {
	linphonePtr plugin(m_plugin.lock());
	if (!plugin) {
		throw FB::script_error("The plugin is invalid");
	}
	return plugin;
}

boost::shared_ptr<LinphoneCallAPI> linphoneAPI::invite(const std::string &dest) {
	//boost::mutex::scoped_lock scopedLock(m_core_mutex);

	FBLOG_DEBUG("linphoneAPI::invite", dest);
	LinphoneCall *call = linphone_core_invite(m_lin_core, dest.c_str());
	boost::shared_ptr<LinphoneCallAPI> shared_call = LinphoneCallAPI::get(call);
	return shared_call;
}

void linphoneAPI::terminate_call(boost::shared_ptr<LinphoneCallAPI> call) {
	//boost::mutex::scoped_lock scopedLock(m_core_mutex);

	FBLOG_DEBUG("linphoneAPI::terminate_call", call);
	linphone_core_terminate_call(m_lin_core, call->getRef());
}

void linphoneAPI::set_play_level(int level) {
	//boost::mutex::scoped_lock scopedLock(m_core_mutex);

	FBLOG_DEBUG("linphoneAPI::set_play_level", level);
	linphone_core_set_play_level(m_lin_core, level);
}

void linphoneAPI::set_rec_level(int level) {
	//boost::mutex::scoped_lock scopedLock(m_core_mutex);

	FBLOG_DEBUG("linphoneAPI::set_rec_level", level);
	linphone_core_set_rec_level(m_lin_core, level);
}

std::string linphoneAPI::getVersion() {
	return linphone_core_get_version();
}

int linphoneAPI::getSipPort() {
	return linphone_core_get_sip_port(m_lin_core);
}

///////////////////////////////////////////////////////////////////////////////
// Wrappers
///////////////////////////////////////////////////////////////////////////////

// Global callbacks which wraps linphoneAPI object methods
#define GLC_DEFINED() (linphone_core_get_user_data(lc) != NULL)
#define GLC(X) ((linphoneAPI *)linphone_core_get_user_data(lc))->X

void linphoneAPI::wrapper_global_state_changed(LinphoneCore *lc, LinphoneGlobalState gstate, const char *message) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "gstate = " << gstate << ", ";
		ss << "message = " << message;
		FBLOG_DEBUG("wrapper_global_state_changed", ss.str());
#endif //DEBUG_LOG
		GLC(fire_global_state_changed(gstate, message));
	}
}
void linphoneAPI::wrapper_registration_state_changed(LinphoneCore *lc, LinphoneProxyConfig *cfg, LinphoneRegistrationState cstate, const char *message) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "cfg = " << cfg << ", ";
		ss << "cstate = " << cstate << ", ";
		ss << "message = " << message;
		FBLOG_DEBUG("wrapper_registration_state_changed", ss.str());
#endif //DEBUG_LOG
	}
}
void linphoneAPI::wrapper_call_state_changed(LinphoneCore *lc, LinphoneCall *call, LinphoneCallState cstate, const char *message) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "call = " << call << ", ";
		ss << "cstate = " << cstate << ", ";
		ss << "message = " << message;
		FBLOG_DEBUG("wrapper_call_state_changed", ss.str());
#endif //DEBUG_LOG
		GLC(fire_call_state_changed(LinphoneCallAPI::get(call), cstate, message));
	}
}
void linphoneAPI::wrapper_notify_presence_recv(LinphoneCore *lc, LinphoneFriend * lf) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "lf = " << lf;
		FBLOG_DEBUG("wrapper_notify_presence_recv", ss.str());
#endif //DEBUG_LOG
	}
}
void linphoneAPI::wrapper_new_subscription_request(LinphoneCore *lc, LinphoneFriend *lf, const char *url) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "lf = " << lf << ", ";
		ss << "url = " << url;
		FBLOG_DEBUG("wrapper_new_subscription_request", ss.str());
#endif //DEBUG_LOG
	}
}
void linphoneAPI::wrapper_auth_info_requested(LinphoneCore *lc, const char *realm, const char *username) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "realm = " << realm << ", ";
		ss << "username = " << username;
		FBLOG_DEBUG("wrapper_auth_info_requested", ss.str());
#endif //DEBUG_LOG
	}
}
void linphoneAPI::wrapper_call_log_updated(LinphoneCore *lc, LinphoneCallLog *newcl) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "newcl = " << newcl;
		FBLOG_DEBUG("wrapper_call_log_updated", ss.str());
#endif //DEBUG_LOG
	}
}
void linphoneAPI::wrapper_text_received(LinphoneCore *lc, LinphoneChatRoom *room, const LinphoneAddress *from, const char *message) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "room = " << room << ", ";
		ss << "from = " << from << ", ";
		ss << "message = " << message;
		FBLOG_DEBUG("wrapper_text_received", ss.str());
#endif //DEBUG_LOG
	}
}
void linphoneAPI::wrapper_dtmf_received(LinphoneCore *lc, LinphoneCall *call, int dtmf) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "call = " << call << ", ";
		ss << "dtmf = " << dtmf;
		FBLOG_DEBUG("wrapper_dtmf_received", ss.str());
#endif //DEBUG_LOG
	}
}
void linphoneAPI::wrapper_refer_received(LinphoneCore *lc, const char *refer_to) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "refer_to = " << refer_to;
		FBLOG_DEBUG("wrapper_refer_received", ss.str());
#endif //DEBUG_LOG
	}
}
void linphoneAPI::wrapper_buddy_info_updated(LinphoneCore *lc, LinphoneFriend *lf) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "lf = " << lf;
		FBLOG_DEBUG("wrapper_buddy_info_updated", ss.str());
#endif //DEBUG_LOG
	}
}
void linphoneAPI::wrapper_notify_recv(LinphoneCore *lc, LinphoneCall *call, const char *from, const char *event) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "call = " << call << ", ";
		ss << "from = " << from << ", ";
		ss << "event = " << event;
		FBLOG_DEBUG("wrapper_notify_recv", ss.str());
#endif //DEBUG_LOG
	}
}
void linphoneAPI::wrapper_display_status(LinphoneCore *lc, const char *message) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "message = " << message;
		FBLOG_DEBUG("wrapper_display_status", ss.str());
#endif //DEBUG_LOG
		GLC(fire_display_status(message));
	}
}
void linphoneAPI::wrapper_display_message(LinphoneCore *lc, const char *message) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "message = " << message;
		FBLOG_DEBUG("wrapper_display_message", ss.str());
#endif //DEBUG_LOG
		GLC(fire_display_message(message));
	}
}
void linphoneAPI::wrapper_display_warning(LinphoneCore *lc, const char *message) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "message = " << message;
		FBLOG_DEBUG("wrapper_display_warning", ss.str());
#endif //DEBUG_LOG
		GLC(fire_display_warning(message));
	}
}
void linphoneAPI::wrapper_display_url(LinphoneCore *lc, const char *message, const char *url) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "message = " << message << ", ";
		ss << "url = " << url;
		FBLOG_DEBUG("wrapper_display_url", ss.str());
#endif //DEBUG_LOG
		GLC(fire_display_url(message, url));
	}
}
void linphoneAPI::wrapper_show(LinphoneCore *lc) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		FBLOG_DEBUG("wrapper_show", ss.str());
#endif //DEBUG_LOG
		GLC(fire_show());
	}
}
void linphoneAPI::wrapper_call_encryption_changed(LinphoneCore *lc, LinphoneCall *call, bool_t on, const char *authentication_token) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "call = " << call << ", ";
		ss << "on = " << on << ", ";
		ss << "authentication_token = " << authentication_token;
		FBLOG_DEBUG("wrapper_call_encryption_changed", ss.str());
#endif //DEBUG_LOG
	}
}
