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

	int setServerAddr(const std::string &server_addr);
	std::string getServerAddr() const;

	int setIdentity(const std::string &identity);
	std::string getIdentity() const;

	int setRoute(const std::string &route);
	std::string getRoute() const;

	void setExpires(int expires);
	int getExpires() const;

	void enableRegister(bool val);
	bool registerEnabled() const;

	int getState();

	void edit();
	int done();

	inline LinphoneProxyConfig *getRef() const{
		return mProxyConfig;
	}
	static boost::shared_ptr<ProxyConfigAPI> get(LinphoneProxyConfig *proxyConfig);
};

#endif //H_PROXYCONFIGAPI
