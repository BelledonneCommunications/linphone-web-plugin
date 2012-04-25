#include "callapi.h"

CallAPI::CallAPI(LinphoneCall *call) :
		mCall(call) {
	FBLOG_DEBUG("CallAPI::CallAPI", this);
	linphone_call_ref(mCall);
	linphone_call_set_user_pointer(mCall, this);

	registerMethod("getState", FB::make_method(this, &CallAPI::getState));
	registerMethod("getRemoteAddress", FB::make_method(this, &CallAPI::getRemoteAddress));
	registerMethod("getDir", FB::make_method(this, &CallAPI::getDir));
}

CallAPI::~CallAPI() {
	FBLOG_DEBUG("CallAPI::~CallAPI", this);
	linphone_call_set_user_pointer(mCall, NULL);
	linphone_call_unref(mCall);
}

int CallAPI::getState() const {
	return linphone_call_get_state(mCall);
}

std::string CallAPI::getRemoteAddress() const {
	const char *txt = linphone_call_get_remote_address_as_string(mCall);
	return txt != NULL ? txt : "";
}

int CallAPI::getDir() const {
	return linphone_call_get_dir(mCall);
}

boost::shared_ptr<CallAPI> CallAPI::get(LinphoneCall *call) {
	void *ptr = linphone_call_get_user_pointer(call);
	boost::shared_ptr<CallAPI> shared_ptr;
	if (ptr == NULL) {
		shared_ptr = boost::shared_ptr<CallAPI>(new CallAPI(call));
	} else {
		shared_ptr = boost::static_pointer_cast<CallAPI>(reinterpret_cast<CallAPI *>(ptr)->shared_from_this());
	}
	return shared_ptr;
}
