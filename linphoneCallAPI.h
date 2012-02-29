#ifndef H_LINPHONECALLAPI
#define H_LINPHONECALLAPI

#include <JSAPIAuto.h>
#include <linphonecore.h>

class LinphoneCallAPI : public FB::JSAPIAuto {
private:
	LinphoneCall *mCall;

public:
	LinphoneCallAPI(LinphoneCall *call);
	~LinphoneCallAPI();
	int get_state();
	LinphoneCall *getRef();
	static boost::shared_ptr<LinphoneCallAPI> get(LinphoneCall *call);
};

#endif //H_LINPHONECALLAPI
