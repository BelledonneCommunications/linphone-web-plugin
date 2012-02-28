/*
 * LinphoneCallAPI.cpp
 *
 */

#include "LinphoneCallAPI.h"

LinphoneCallAPI::LinphoneCallAPI(LinphoneCall *call) :
		mCall(call) {
	FBLOG_DEBUG("LinphoneCallAPI::LinphoneCallAPI", this);
	linphone_call_ref(mCall);
	linphone_call_set_user_pointer(mCall, this);

	registerMethod("get_state", FB::make_method(this, &LinphoneCallAPI::get_state));
}

LinphoneCallAPI::~LinphoneCallAPI() {
	FBLOG_DEBUG("LinphoneCallAPI::~LinphoneCallAPI", this);
	linphone_call_unref(mCall);
	linphone_call_set_user_pointer(mCall, NULL);
}


int LinphoneCallAPI::get_state() {
	return linphone_call_get_state(mCall);
}

LinphoneCall *LinphoneCallAPI::getRef() {
	return mCall;
}

boost::shared_ptr<LinphoneCallAPI> LinphoneCallAPI::get(LinphoneCall *call) {
	void *ptr = linphone_call_get_user_pointer(call);
	if(ptr == NULL)  {
		ptr = new LinphoneCallAPI(call);
	}
	return boost::shared_ptr<LinphoneCallAPI>(reinterpret_cast<LinphoneCallAPI *>(ptr));
}
