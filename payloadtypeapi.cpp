#include "payloadtypeapi.h"
#include "coreapi.h"

PayloadTypeAPI::PayloadTypeAPI(const boost::shared_ptr<CoreAPI> &core, PayloadType *payloadType) :
		mCore(core), mPayloadType(payloadType) {
	FBLOG_DEBUG("PayloadTypeAPI::PayloadTypeAPI", this);
	mPayloadType->user_data = this;

	registerProperty("type", make_property(this, &PayloadTypeAPI::get_type));
	registerProperty("clock_rate", make_property(this, &PayloadTypeAPI::get_clock_rate));
	registerProperty("bits_per_sample", make_property(this, &PayloadTypeAPI::get_bits_per_sample));
	registerProperty("zero_pattern", make_property(this, &PayloadTypeAPI::get_zero_pattern));
	registerProperty("pattern_length", make_property(this, &PayloadTypeAPI::get_pattern_length));
	registerProperty("normal_bitrate", make_property(this, &PayloadTypeAPI::get_normal_bitrate));
	registerProperty("mime_type", make_property(this, &PayloadTypeAPI::get_mime_type));
	registerProperty("channels", make_property(this, &PayloadTypeAPI::get_channels));
	registerProperty("recv_fmtp", make_property(this, &PayloadTypeAPI::get_recv_fmtp));
	registerProperty("send_fmtp", make_property(this, &PayloadTypeAPI::get_send_fmtp));
	registerProperty("flags", make_property(this, &PayloadTypeAPI::get_flags));
	registerProperty("enabled", make_property(this, &PayloadTypeAPI::get_enabled, &PayloadTypeAPI::set_enabled));
}

bool PayloadTypeAPI::get_enabled() const {
	FBLOG_DEBUG("PayloadTypeAPI::get_enabled()", this);
	boost::shared_ptr<CoreAPI> core(mCore.lock());
	if (core != NULL) {
		return linphone_core_payload_type_enabled(core->getRef(), mPayloadType);
	}
	return false;
}

void PayloadTypeAPI::set_enabled(bool enable) {
	FBLOG_DEBUG("PayloadTypeAPI::set_enabled()", this);
	boost::shared_ptr<CoreAPI> core(mCore.lock());
	if (core != NULL) {
		linphone_core_enable_payload_type(core->getRef(), mPayloadType, enable);
	}
}

PayloadTypeAPI::~PayloadTypeAPI() {
	FBLOG_DEBUG("ProxyConfigAPI::~ProxyConfigAPI", this);
	mPayloadType->user_data = NULL;
}

boost::shared_ptr<PayloadTypeAPI> PayloadTypeAPI::get(const boost::shared_ptr<CoreAPI> &core, PayloadType *payloadType) {
	void *ptr = payloadType->user_data;
	boost::shared_ptr<PayloadTypeAPI> shared_ptr;
	if (ptr == NULL) {
		shared_ptr = boost::shared_ptr<PayloadTypeAPI>(new PayloadTypeAPI(core, payloadType));
	} else {
		shared_ptr = boost::static_pointer_cast<PayloadTypeAPI>(reinterpret_cast<PayloadTypeAPI *>(ptr)->shared_from_this());
	}
	return shared_ptr;
}
