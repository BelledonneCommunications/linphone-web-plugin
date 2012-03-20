#ifndef H_PAYLOADTYPEAPI
#define H_PAYLOADTYPEAPI

#include <JSAPIAuto.h>
#include <linphonecore.h>

class CoreAPI;
class PayloadTypeAPI: public FB::JSAPIAuto {
private:
	boost::weak_ptr<CoreAPI> mCore;
	PayloadType *mPayloadType;

	PayloadTypeAPI(const boost::shared_ptr<CoreAPI> &core, PayloadType *payloadType);
public:
	~PayloadTypeAPI();

	inline int get_type() const {
		return mPayloadType->type;
	}
	inline int get_clock_rate() const {
		return mPayloadType->clock_rate;
	}
	inline int get_bits_per_sample() const {
		return mPayloadType->bits_per_sample;
	}
	inline std::string get_zero_pattern() const {
		return mPayloadType->zero_pattern;
	}
	inline int get_pattern_length() const {
		return mPayloadType->pattern_length;
	}
	inline int get_normal_bitrate() const {
		return mPayloadType->normal_bitrate;
	}
	inline std::string get_mime_type() const {
		return mPayloadType->mime_type;
	}
	inline int get_channels() const {
		return mPayloadType->channels;
	}
	inline std::string get_recv_fmtp() const {
		return mPayloadType->recv_fmtp;
	}
	inline std::string get_send_fmtp() const {
		return mPayloadType->send_fmtp;
	}
	inline int get_flags() const {
		return mPayloadType->flags;
	}

	bool get_enabled() const;

	void set_enabled(bool enable);

	inline PayloadType *getRef() const {
		return mPayloadType;
	}
	static boost::shared_ptr<PayloadTypeAPI> get(const boost::shared_ptr<CoreAPI> &core, PayloadType *payloadType);
};

#endif //H_PAYLOADTYPEAPI
