#ifndef H_PROXYCONFIGAPI
#define H_PROXYCONFIGAPI

#include <JSAPIAuto.h>
#include <linphonecore.h>

class ProxyConfigAPI : public FB::JSAPIAuto {
private:
	LinphoneProxyConfig *mProxyConfig;

	ProxyConfigAPI(LinphoneProxyConfig *proxyConfig);
public:
	~ProxyConfigAPI();
	inline LinphoneProxyConfig *getRef() const{
		return mProxyConfig;
	}
	static boost::shared_ptr<ProxyConfigAPI> get(LinphoneProxyConfig *proxyConfig);
};

#endif //H_PROXYCONFIGAPI
