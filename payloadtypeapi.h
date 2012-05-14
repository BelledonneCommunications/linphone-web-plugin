/*!
 Linphone Web - Web plugin of Linphone an audio/video SIP phone
 Copyright (C) 2012  Yann Diorcet <yann.diorcet@linphone.org>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

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
