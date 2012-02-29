#include "linphoneCallAPI.h"

LinphoneCallAPI::LinphoneCallAPI(LinphoneCall *call) :
		mCall(call) {
	FBLOG_DEBUG("LinphoneCallAPI::LinphoneCallAPI", this);
	linphone_call_ref(mCall);
	linphone_call_set_user_pointer(mCall, this);

	registerMethod("get_state", FB::make_method(this, &LinphoneCallAPI::get_state));
}

LinphoneCallAPI::~LinphoneCallAPI() {
	FBLOG_DEBUG("LinphoneCallAPI::~LinphoneCallAPI", this);
	linphone_call_set_user_pointer(mCall, NULL);
	linphone_call_unref(mCall);
}

int LinphoneCallAPI::get_state() {
	return linphone_call_get_state(mCall);
}

LinphoneCall *LinphoneCallAPI::getRef() {
	return mCall;
}

boost::shared_ptr<LinphoneCallAPI> LinphoneCallAPI::get(LinphoneCall *call) {
	void *ptr = linphone_call_get_user_pointer(call);
	boost::shared_ptr<LinphoneCallAPI> shared_ptr;
	if (ptr == NULL) {
		shared_ptr = boost::make_shared<LinphoneCallAPI>(call);
	} else {
		shared_ptr = boost::static_pointer_cast<LinphoneCallAPI>(reinterpret_cast<LinphoneCallAPI *>(ptr)->shared_from_this());
	}
	return shared_ptr;
}
