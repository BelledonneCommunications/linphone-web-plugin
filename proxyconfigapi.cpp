#include "proxyconfigapi.h"

ProxyConfigAPI::ProxyConfigAPI(LinphoneProxyConfig *proxyConfig) :
		mProxyConfig(proxyConfig) {
	FBLOG_DEBUG("ProxyConfigAPI::ProxyConfigAPI", this);
	linphone_proxy_config_set_user_data(mProxyConfig, this);
	init_proxy();
}

ProxyConfigAPI::ProxyConfigAPI() {
	FBLOG_DEBUG("ProxyConfigAPI::ProxyConfigAPI", this);
	mProxyConfig = linphone_proxy_config_new();
	linphone_proxy_config_set_user_data(mProxyConfig, this);
	init_proxy();
}

void ProxyConfigAPI::init_proxy() {
	registerMethod("set_server_addr", make_method(this, &ProxyConfigAPI::set_server_addr));
	registerMethod("get_server_addr", make_method(this, &ProxyConfigAPI::get_server_addr));

	registerMethod("set_identity", make_method(this, &ProxyConfigAPI::set_identity));
	registerMethod("get_identity", make_method(this, &ProxyConfigAPI::get_identity));

	registerMethod("set_route", make_method(this, &ProxyConfigAPI::set_route));
	registerMethod("get_route", make_method(this, &ProxyConfigAPI::get_route));

	registerMethod("set_expires", make_method(this, &ProxyConfigAPI::set_expires));
	registerMethod("get_expires", make_method(this, &ProxyConfigAPI::get_expires));

	registerMethod("enable_register", make_method(this, &ProxyConfigAPI::enable_register));
	registerMethod("register_enabled", make_method(this, &ProxyConfigAPI::register_enabled));
}

ProxyConfigAPI::~ProxyConfigAPI() {
	FBLOG_DEBUG("ProxyConfigAPI::~ProxyConfigAPI", this);
	linphone_proxy_config_set_user_data(mProxyConfig, NULL);
}

int ProxyConfigAPI::set_server_addr(const std::string &server_addr) {
	FBLOG_DEBUG("ProxyConfigAPI::set_server_addr()", "server_addr=" << server_addr);
	return linphone_proxy_config_set_server_addr(mProxyConfig, server_addr.c_str());
}
std::string ProxyConfigAPI::get_server_addr() {
	FBLOG_DEBUG("ProxyConfigAPI::get_server_addr()", "");
	return linphone_proxy_config_get_addr(mProxyConfig);
}

int ProxyConfigAPI::set_identity(const std::string &identity) {
	FBLOG_DEBUG("ProxyConfigAPI::set_identity()", "identity=" << identity);
	return linphone_proxy_config_set_identity(mProxyConfig, identity.c_str());
}
std::string ProxyConfigAPI::get_identity() {
	FBLOG_DEBUG("ProxyConfigAPI::get_identity()", "");
	return linphone_proxy_config_get_identity(mProxyConfig);
}

int ProxyConfigAPI::set_route(const std::string &route) {
	FBLOG_DEBUG("ProxyConfigAPI::set_route()", "route=" << route);
	return linphone_proxy_config_set_route(mProxyConfig, route.c_str());
}
std::string ProxyConfigAPI::get_route() {
	FBLOG_DEBUG("ProxyConfigAPI::get_route()", "");
	return linphone_proxy_config_get_route(mProxyConfig);
}

void ProxyConfigAPI::set_expires(int expires) {
	FBLOG_DEBUG("ProxyConfigAPI::expires()", "expires=" << expires);
	return linphone_proxy_config_expires(mProxyConfig, expires);
}
int ProxyConfigAPI::get_expires() {
	FBLOG_DEBUG("ProxyConfigAPI::get_expires()", "");
	return linphone_proxy_config_get_expires(mProxyConfig);
}

void ProxyConfigAPI::enable_register(bool val) {
	FBLOG_DEBUG("ProxyConfigAPI::enable_register()", "val=" << val);
	return linphone_proxy_config_enable_register(mProxyConfig, val ? TRUE : FALSE);
}
bool ProxyConfigAPI::register_enabled() {
	FBLOG_DEBUG("ProxyConfigAPI::register_enabled()", "");
	return linphone_proxy_config_register_enabled(mProxyConfig) == TRUE? true: false;
}

void ProxyConfigAPI::edit() {
	FBLOG_DEBUG("ProxyConfigAPI::edit()", "");
	linphone_proxy_config_edit(mProxyConfig);
}

int ProxyConfigAPI::done() {
	return linphone_proxy_config_done(mProxyConfig);
}

boost::shared_ptr<ProxyConfigAPI> ProxyConfigAPI::get(LinphoneProxyConfig *proxyConfig) {
	void *ptr = linphone_proxy_config_get_user_data(proxyConfig);
	boost::shared_ptr<ProxyConfigAPI> shared_ptr;
	if (ptr == NULL) {
		shared_ptr = boost::shared_ptr<ProxyConfigAPI>(new ProxyConfigAPI(proxyConfig));
	} else {
		shared_ptr = boost::static_pointer_cast<ProxyConfigAPI>(reinterpret_cast<ProxyConfigAPI *>(ptr)->shared_from_this());
	}
	return shared_ptr;
}
