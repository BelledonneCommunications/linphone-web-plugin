#ifndef H_PROXYCONFIGAPI
#define H_PROXYCONFIGAPI

#include <JSAPIAuto.h>
#include <linphonecore.h>

class ProxyConfigAPI : public FB::JSAPIAuto {
private:
	LinphoneProxyConfig *mProxyConfig;

	ProxyConfigAPI(LinphoneProxyConfig *proxyConfig);
	void init_proxy();
public:
	ProxyConfigAPI();
	~ProxyConfigAPI();

	int set_server_addr(const std::string &server_addr);
	std::string get_server_addr();

	int set_identity(const std::string &identity);
	std::string get_identity();

	int set_route(const std::string &route);
	std::string get_route();

	void set_expires(int expires);
	int get_expires();

	void enable_register(bool val);
	bool register_enabled();

	void edit();
	int done();

	inline LinphoneProxyConfig *getRef() const{
		return mProxyConfig;
	}
	static boost::shared_ptr<ProxyConfigAPI> get(LinphoneProxyConfig *proxyConfig);
};

#endif //H_PROXYCONFIGAPI
