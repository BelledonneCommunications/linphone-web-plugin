#ifndef H_CALLAPI
#define H_CALLAPI

#include <JSAPIAuto.h>
#include <linphonecore.h>

class CallAPI : public FB::JSAPIAuto {
private:
	LinphoneCall *mCall;
	CallAPI(LinphoneCall *call);

public:
	~CallAPI();
	std::string getRemoteAddress() const;
	int getDir() const;
	int getState() const;
	inline LinphoneCall *getRef() const{
		return mCall;
	}
	static boost::shared_ptr<CallAPI> get(LinphoneCall *call);
};

#endif //H_CALLAPI
