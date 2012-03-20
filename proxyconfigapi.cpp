#include "proxyconfigapi.h"

ProxyConfigAPI::ProxyConfigAPI(LinphoneProxyConfig *proxyConfig) :
		mProxyConfig(proxyConfig) {
	FBLOG_DEBUG("ProxyConfigAPI::ProxyConfigAPI", this);
	linphone_proxy_config_set_user_data(mProxyConfig, this);

}

ProxyConfigAPI::~ProxyConfigAPI() {
	FBLOG_DEBUG("ProxyConfigAPI::~ProxyConfigAPI", this);
	linphone_proxy_config_set_user_data(mProxyConfig, NULL);
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
