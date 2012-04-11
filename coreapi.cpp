#include <JSObject.h>
#include <variant_list.h>
#include <DOM/Document.h>
#include <global/config.h>
#include <sstream>
#include <boost/foreach.hpp>
#include "coreapi.h"
#ifndef WIN32
#else
#include <windows.h>
void usleep(int waitTime) {
	Sleep(waitTime/1000);
}
#endif
#define CORE_MUTEX ;
//#define CORE_MUTEX boost::mutex::scoped_lock scopedLock(m_core_mutex);

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
/// @fn CoreAPI::CoreAPI(const linphonePtr& plugin, const FB::BrowserHostPtr host)
///
/// @brief  Constructor for your JSAPI object.  You should register your methods, properties, and events
///         that should be accessible to Javascript from here.
///
/// @see FB::JSAPIAuto::registerMethod
/// @see FB::JSAPIAuto::registerProperty
/// @see FB::JSAPIAuto::registerEvent
///////////////////////////////////////////////////////////////////////////////
CoreAPI::CoreAPI(const linphonePtr& plugin, const FB::BrowserHostPtr& host) :
		m_plugin(plugin), m_host(host), m_lin_core(NULL), m_core_thread(NULL), m_threads(new mythread_group()) {
	FBLOG_DEBUG("CoreAPI::CoreAPI()", this);

	// Read-only property
	registerProperty("version", make_property(this, &CoreAPI::getVersion));
	registerProperty("sip_port", make_property(this, &CoreAPI::getSipPort));

	// Propery
	registerProperty("magic", make_property(this, &CoreAPI::getMagic, &CoreAPI::setMagic));

	// Methods
	registerMethod("init", make_method(this, &CoreAPI::init));
	REGISTER_SYNC_N_ASYNC("invite", invite);

	registerMethod("accept_call", make_method(this, &CoreAPI::accept_call));
	registerMethod("terminate_call", make_method(this, &CoreAPI::terminate_call));

	registerMethod("set_play_level", make_method(this, &CoreAPI::set_play_level));
	registerMethod("set_rec_level", make_method(this, &CoreAPI::set_rec_level));
	registerMethod("set_ring_level", make_method(this, &CoreAPI::set_ring_level));

	registerMethod("video_supported", make_method(this, &CoreAPI::video_supported));
	registerMethod("enable_video", make_method(this, &CoreAPI::enable_video));
	registerMethod("video_enabled", make_method(this, &CoreAPI::video_enabled));
	registerMethod("enable_video_preview", make_method(this, &CoreAPI::enable_video_preview));
	registerMethod("video_preview_enabled", make_method(this, &CoreAPI::video_preview_enabled));
	registerMethod("get_native_video_window_id", make_method(this, &CoreAPI::get_native_video_window_id));
	registerMethod("set_native_video_window_id", make_method(this, &CoreAPI::set_native_video_window_id));
	registerMethod("get_native_preview_window_id", make_method(this, &CoreAPI::get_native_preview_window_id));
	registerMethod("set_native_preview_window_id", make_method(this, &CoreAPI::set_native_preview_window_id));

	registerMethod("get_audio_codecs", make_method(this, &CoreAPI::get_audio_codecs));
	registerMethod("get_video_codecs", make_method(this, &CoreAPI::get_video_codecs));
	registerMethod("set_audio_codecs", make_method(this, &CoreAPI::set_audio_codecs));
	registerMethod("set_video_codecs", make_method(this, &CoreAPI::set_video_codecs));

	registerMethod("new_proxy_config", make_method(this, &CoreAPI::new_proxy_config));
	registerMethod("add_proxy_config", make_method(this, &CoreAPI::add_proxy_config));
	registerMethod("clear_proxy_config", make_method(this, &CoreAPI::clear_proxy_config));
	registerMethod("remove_proxy_config", make_method(this, &CoreAPI::remove_proxy_config));
	registerMethod("get_proxy_config_list", make_method(this, &CoreAPI::get_proxy_config_list));
	registerMethod("set_default_proxy", make_method(this, &CoreAPI::set_default_proxy));
	registerMethod("get_default_proxy", make_method(this, &CoreAPI::get_default_proxy));
}

int CoreAPI::init() {
	FBLOG_DEBUG("CoreAPI::init()", this);
	boost::mutex::scoped_lock scoped_instance_count_lock(sInstanceMutex);

	if (sInstanceCount == 0) {
		++sInstanceCount;
		srand((unsigned int) time(NULL));

		// Disable logs
		linphone_core_disable_logs();

		// Initialize callback table
		memset(&m_lin_vtable, 0, sizeof(LinphoneCoreVTable));
		m_lin_vtable.global_state_changed = CoreAPI::wrapper_global_state_changed;
		m_lin_vtable.registration_state_changed = CoreAPI::wrapper_registration_state_changed;
		m_lin_vtable.call_state_changed = CoreAPI::wrapper_call_state_changed;
		m_lin_vtable.notify_presence_recv = CoreAPI::wrapper_notify_presence_recv;
		m_lin_vtable.new_subscription_request = CoreAPI::wrapper_new_subscription_request;
		m_lin_vtable.auth_info_requested = CoreAPI::wrapper_auth_info_requested;
		m_lin_vtable.call_log_updated = CoreAPI::wrapper_call_log_updated;
		m_lin_vtable.text_received = CoreAPI::wrapper_text_received;
		m_lin_vtable.dtmf_received = CoreAPI::wrapper_dtmf_received;
		m_lin_vtable.refer_received = CoreAPI::wrapper_refer_received;
		m_lin_vtable.buddy_info_updated = CoreAPI::wrapper_buddy_info_updated;
		m_lin_vtable.notify_recv = CoreAPI::wrapper_notify_recv;
		m_lin_vtable.display_status = CoreAPI::wrapper_display_status;
		m_lin_vtable.display_message = CoreAPI::wrapper_display_message;
		m_lin_vtable.display_warning = CoreAPI::wrapper_display_warning;
		m_lin_vtable.display_url = CoreAPI::wrapper_display_url;
		m_lin_vtable.show = CoreAPI::wrapper_show;
		m_lin_vtable.call_encryption_changed = CoreAPI::wrapper_call_encryption_changed;

		m_lin_core = linphone_core_new(&m_lin_vtable, NULL, NULL, (void *) this);
		if (linphone_core_get_user_data(m_lin_core) != this) {
			FBLOG_ERROR("CoreAPI::init()", "Too old version of linphone core!");
			--sInstanceCount;
			return 1;
		}

		for (const MSList *node = linphone_core_get_audio_codecs(m_lin_core); node != NULL; node = ms_list_next(node)) {
			reinterpret_cast<PayloadType*>(node->data)->user_data = NULL;
		}

		for (const MSList *node = linphone_core_get_video_codecs(m_lin_core); node != NULL; node = ms_list_next(node)) {
			reinterpret_cast<PayloadType*>(node->data)->user_data = NULL;
		}

		int port = 5000 + rand() % 5000;
		FBLOG_DEBUG("CoreAPI::init()", "port=" << port);
		linphone_core_set_sip_port(m_lin_core, port);

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
	FBLOG_DEBUG("CoreAPI::~CoreAPI()", this);
	if (m_lin_core != NULL) {
		linphone_core_set_user_data(m_lin_core, NULL);

		boost::thread t(linphone_destroy_thread, m_lin_core, m_core_thread, m_threads);
	}
}

///////////////////////////////////////////////////////////////////////////////
/// @fn linphonePtr CoreAPI::getPlugin()
///
/// @brief  Gets a reference to the plugin that was passed in when the object
///         was created.  If the plugin has already been released then this
///         will throw a FB::script_error that will be translated into a
///         javascript exception in the page.
///////////////////////////////////////////////////////////////////////////////
linphonePtr CoreAPI::getPlugin() {
	linphonePtr plugin(m_plugin.lock());
	if (!plugin) {
		throw FB::script_error("The plugin is invalid");
	}
	return plugin;
}

const std::string &CoreAPI::getMagic() {
	FBLOG_DEBUG("CoreAPI::getMagic()", "");
	return m_magic;
}

void CoreAPI::setMagic(const std::string &magic) {
	FBLOG_DEBUG("CoreAPI::setMagic()", "magic=" << magic);
	m_magic = magic;
}

boost::shared_ptr<CallAPI> CoreAPI::invite(const std::string &dest) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::invite", "dest=" << dest);
	LinphoneCall *call = linphone_core_invite(m_lin_core, dest.c_str());
	boost::shared_ptr<CallAPI> shared_call = CallAPI::get(call);
	return shared_call;
}

void CoreAPI::terminate_call(const boost::shared_ptr<CallAPI> &call) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::terminate_call", "call=" << call);
	linphone_core_terminate_call(m_lin_core, call->getRef());
}

void CoreAPI::accept_call(const boost::shared_ptr<CallAPI> &call) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::accept_call", "call=" << call);
	linphone_core_accept_call(m_lin_core, call->getRef());
}

void CoreAPI::set_play_level(int level) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::set_play_level", "level=" << level);
	linphone_core_set_play_level(m_lin_core, level);
}

void CoreAPI::set_rec_level(int level) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::set_rec_level", "level=" << level);
	linphone_core_set_rec_level(m_lin_core, level);
}

void CoreAPI::set_ring_level(int level) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::set_ring_level", "level=" << level);
	linphone_core_set_ring_level(m_lin_core, level);
}

bool CoreAPI::video_supported() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::video_supported()", "");
	return linphone_core_video_supported(m_lin_core) == TRUE ? true : false;
}
void CoreAPI::enable_video(bool enable) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enable_video()", "enable=" << enable);
	linphone_core_enable_video(m_lin_core, enable ? TRUE : FALSE, enable ? TRUE : FALSE);
}

bool CoreAPI::video_enabled() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::video_enabled()", "");
	return linphone_core_video_enabled(m_lin_core) == TRUE ? true : false;
}

void CoreAPI::enable_video_preview(bool enable) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::enable_video_preview()", "enable=" << enable);
	linphone_core_enable_video_preview(m_lin_core, enable ? TRUE : FALSE);
}

bool CoreAPI::video_preview_enabled() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::video_preview_enabled()", "");
	return linphone_core_video_preview_enabled(m_lin_core) == TRUE ? true : false;
}

void CoreAPI::set_native_video_window_id(unsigned long id) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::set_native_video_window_id()", "id=" << id);
	linphone_core_set_native_video_window_id(m_lin_core, id);
}

unsigned long CoreAPI::get_native_video_window_id() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::set_native_video_window_id()", "");
	return linphone_core_get_native_video_window_id(m_lin_core);
}


void CoreAPI::set_native_preview_window_id(unsigned long id) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::set_native_preview_window_id()", "id=" << id);
	linphone_core_set_native_preview_window_id(m_lin_core, id);
}

unsigned long CoreAPI::get_native_preview_window_id() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::get_native_preview_window_id()", "");
	return linphone_core_get_native_preview_window_id(m_lin_core);
}

FB::VariantList CoreAPI::get_audio_codecs() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::get_audio_codecs()", "");
	FB::VariantList list;
	for (const MSList *node = linphone_core_get_audio_codecs(m_lin_core); node != NULL; node = ms_list_next(node)) {
		list.push_back(PayloadTypeAPI::get(boost::static_pointer_cast<CoreAPI>(shared_from_this()), reinterpret_cast<PayloadType*>(node->data)));
	}
	return list;
}

FB::VariantList CoreAPI::get_video_codecs() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::get_video_codecs()", "");
	FB::VariantList list;
	for (const MSList *node = linphone_core_get_video_codecs(m_lin_core); node != NULL; node = ms_list_next(node)) {
		list.push_back(PayloadTypeAPI::get(boost::static_pointer_cast<CoreAPI>(shared_from_this()), reinterpret_cast<PayloadType*>(node->data)));
	}
	return list;
}

void CoreAPI::set_audio_codecs(const std::vector<boost::shared_ptr<FB::JSAPI> > &list) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::set_video_codecs()", "list.size()=" << list.size());
	MSList *mslist = NULL;
	for (auto it = list.begin(); it != list.end(); ++it) {
		boost::shared_ptr<PayloadTypeAPI> payloadType = boost::dynamic_pointer_cast<PayloadTypeAPI>(*it);
		if (payloadType != NULL) {
			mslist = ms_list_append(mslist, payloadType->getRef());
		} else {
			FBLOG_DEBUG("CoreAPI::set_video_codecs()", "Not PayloadTypeAPI");
		}
	}

	linphone_core_set_audio_codecs(m_lin_core, mslist);
}

void CoreAPI::set_video_codecs(const std::vector<boost::shared_ptr<FB::JSAPI> > &list) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::set_video_codecs()", "list.size()=" << list.size());
	MSList *mslist = NULL;
	for (auto it = list.begin(); it != list.end(); ++it) {
		boost::shared_ptr<PayloadTypeAPI> payloadType = boost::dynamic_pointer_cast<PayloadTypeAPI>(*it);
		if (payloadType != NULL) {
			mslist = ms_list_append(mslist, payloadType->getRef());
		} else {
			FBLOG_DEBUG("CoreAPI::set_video_codecs()", "Not PayloadTypeAPI");
		}
	}

	linphone_core_set_video_codecs(m_lin_core, mslist);
}

boost::shared_ptr<ProxyConfigAPI> CoreAPI::new_proxy_config() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::new_proxy_config()", "");
	return boost::make_shared<ProxyConfigAPI>();
}
int CoreAPI::add_proxy_config(const boost::shared_ptr<ProxyConfigAPI> &config) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::add_proxy_config()", "config=" << config);
	return linphone_core_add_proxy_config(m_lin_core, config->getRef());
}
void CoreAPI::clear_proxy_config() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::clear_proxy_config()", "");
	linphone_core_clear_proxy_config(m_lin_core);
}
void CoreAPI::remove_proxy_config(const boost::shared_ptr<ProxyConfigAPI> &config) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::remove_proxy_config()", "config=" << config);
	linphone_core_remove_proxy_config(m_lin_core, config->getRef());
}
FB::VariantList CoreAPI::get_proxy_config_list() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::get_proxy_config_list()", "");
	FB::VariantList list;
	for (const MSList *node = linphone_core_get_proxy_config_list(m_lin_core); node != NULL; node = ms_list_next(node)) {
		list.push_back(ProxyConfigAPI::get(reinterpret_cast<LinphoneProxyConfig*>(node->data)));
	}
	return list;
}
void CoreAPI::set_default_proxy(const boost::shared_ptr<ProxyConfigAPI> &config) {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::set_default_proxy()", "config=" << config);
	linphone_core_set_default_proxy(m_lin_core, config->getRef());
}
boost::shared_ptr<ProxyConfigAPI> CoreAPI::get_default_proxy() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::get_default_proxy()", "");
	LinphoneProxyConfig *ptr = NULL;
	linphone_core_get_default_proxy(m_lin_core, &ptr);
	if(ptr != NULL)
		return ProxyConfigAPI::get(ptr);
	return boost::shared_ptr<ProxyConfigAPI>();
}

std::string CoreAPI::getVersion() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getVersion()", this);
	return linphone_core_get_version();
}

int CoreAPI::getSipPort() {
	CORE_MUTEX

	FBLOG_DEBUG("CoreAPI::getSipPort()", this);
	return linphone_core_get_sip_port(m_lin_core);
}

///////////////////////////////////////////////////////////////////////////////
// Wrappers
///////////////////////////////////////////////////////////////////////////////

// Global callbacks which wraps CoreAPI object methods
#define GLC_DEFINED() (linphone_core_get_user_data(lc) != NULL)
#define GLC(X) ((CoreAPI *)linphone_core_get_user_data(lc))->X

void CoreAPI::wrapper_global_state_changed(LinphoneCore *lc, LinphoneGlobalState gstate, const char *message) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "gstate = " << gstate << ", ";
		ss << "message = " << message;
		FBLOG_DEBUG("wrapper_global_state_changed", ss.str());
#endif //FB_NO_LOGGING_MACROS
		GLC(fire_global_state_changed(gstate, message));
	}
}
void CoreAPI::wrapper_registration_state_changed(LinphoneCore *lc, LinphoneProxyConfig *cfg, LinphoneRegistrationState cstate, const char *message) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "cfg = " << cfg << ", ";
		ss << "cstate = " << cstate << ", ";
		ss << "message = " << message;
		FBLOG_DEBUG("wrapper_registration_state_changed", ss.str());
#endif //FB_NO_LOGGING_MACROS
	}
}
void CoreAPI::wrapper_call_state_changed(LinphoneCore *lc, LinphoneCall *call, LinphoneCallState cstate, const char *message) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "call = " << call << ", ";
		ss << "cstate = " << cstate << ", ";
		ss << "message = " << message;
		FBLOG_DEBUG("wrapper_call_state_changed", ss.str());
#endif //FB_NO_LOGGING_MACROS
		GLC(fire_call_state_changed(CallAPI::get(call), cstate, message));
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
		ss << "realm = " << realm << ", ";
		ss << "username = " << username;
		FBLOG_DEBUG("wrapper_auth_info_requested", ss.str());
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
		ss << "message = " << message;
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
		ss << "refer_to = " << refer_to;
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
		ss << "from = " << from << ", ";
		ss << "event = " << event;
		FBLOG_DEBUG("wrapper_notify_recv", ss.str());
#endif //FB_NO_LOGGING_MACROS
	}
}
void CoreAPI::wrapper_display_status(LinphoneCore *lc, const char *message) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "message = " << message;
		FBLOG_DEBUG("wrapper_display_status", ss.str());
#endif //FB_NO_LOGGING_MACROS
		GLC(fire_display_status(message));
	}
}
void CoreAPI::wrapper_display_message(LinphoneCore *lc, const char *message) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "message = " << message;
		FBLOG_DEBUG("wrapper_display_message", ss.str());
#endif //FB_NO_LOGGING_MACROS
		GLC(fire_display_message(message));
	}
}
void CoreAPI::wrapper_display_warning(LinphoneCore *lc, const char *message) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "message = " << message;
		FBLOG_DEBUG("wrapper_display_warning", ss.str());
#endif //FB_NO_LOGGING_MACROS
		GLC(fire_display_warning(message));
	}
}
void CoreAPI::wrapper_display_url(LinphoneCore *lc, const char *message, const char *url) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		ss << "message = " << message << ", ";
		ss << "url = " << url;
		FBLOG_DEBUG("wrapper_display_url", ss.str());
#endif //FB_NO_LOGGING_MACROS
		GLC(fire_display_url(message, url));
	}
}
void CoreAPI::wrapper_show(LinphoneCore *lc) {
	if (GLC_DEFINED()) {
#if !FB_NO_LOGGING_MACROS
		std::stringstream ss;
		FBLOG_DEBUG("wrapper_show", ss.str());
#endif //FB_NO_LOGGING_MACROS
		GLC(fire_show());
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
