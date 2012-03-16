#ifndef H_CALLAPI
#define H_CALLAPI

#include <JSAPIAuto.h>
#include <linphonecore.h>

class CallAPI : public FB::JSAPIAuto {
private:
	LinphoneCall *mCall;

public:
	CallAPI(LinphoneCall *call);
	~CallAPI();
	int get_state();
	LinphoneCall *getRef();
	static boost::shared_ptr<CallAPI> get(LinphoneCall *call);
};

#endif //H_CALLAPI
