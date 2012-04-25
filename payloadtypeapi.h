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

	inline int getType() const {
		return mPayloadType->type;
	}
	inline int getClockRate() const {
		return mPayloadType->clock_rate;
	}
	inline int getBitsPerSample() const {
		return mPayloadType->bits_per_sample;
	}
	inline std::string getZeroPattern() const {
		return mPayloadType->zero_pattern;
	}
	inline int getPatternLength() const {
		return mPayloadType->pattern_length;
	}
	inline int getNormalBitrate() const {
		return mPayloadType->normal_bitrate;
	}
	inline std::string getMimeType() const {
		return mPayloadType->mime_type;
	}
	inline int getChannels() const {
		return mPayloadType->channels;
	}
	inline std::string getRecvFmtp() const {
		return mPayloadType->recv_fmtp;
	}
	inline std::string getSendFmtp() const {
		return mPayloadType->send_fmtp;
	}
	inline int getFlags() const {
		return mPayloadType->flags;
	}

	bool getEnabled() const;

	void setEnabled(bool enable);

	inline PayloadType *getRef() const {
		return mPayloadType;
	}
	static boost::shared_ptr<PayloadTypeAPI> get(const boost::shared_ptr<CoreAPI> &core, PayloadType *payloadType);
};

#endif //H_PAYLOADTYPEAPI
