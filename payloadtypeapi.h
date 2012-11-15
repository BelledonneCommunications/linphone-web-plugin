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

FB_FORWARD_PTR(CoreAPI);

FB_FORWARD_PTR(PayloadTypeAPI)
class PayloadTypeAPI: public FB::JSAPIAuto {
private:
	PayloadType *mPayloadType;
	bool mConst;

	PayloadTypeAPI(PayloadType *payloadType);
	PayloadTypeAPI(const PayloadType *payloadType);
	void initProxy();
public:
	~PayloadTypeAPI();

	PayloadTypeAPIPtr clone() const;
	std::string getRtpmap() const;

	int getType() const;
	void setType(int type);

	int getClockRate() const;
	void setClockRate(int rate);

	int getBitsPerSample() const;
	void setBitsPerSample(int bps);

	std::string getZeroPattern() const;
	void setZeroPattern(const std::string &pattern);

	int getPatternLength() const;
	void setPatternLength(int length);

	int getNormalBitrate() const;
	void setNormalBitrate(int bitrate);

	std::string getMimeType() const;
	void setMimeType(const std::string &mime);

	int getChannels() const;
	void setChannels(int channels);

	std::string getRecvFmtp() const;
	void setRecvFmtp(const std::string &rfmtp);

	std::string getSendFmtp() const;
	void setSendFmtp(const std::string &sfmtp);

	int getFlags() const;
	void setFlags(int flags);

	inline PayloadType *getRef() const {
		return mPayloadType;
	}
	static PayloadTypeAPIPtr get(PayloadType *payloadType);
	static PayloadTypeAPIPtr get(const PayloadType *payloadType);
};

#endif //H_PAYLOADTYPEAPI
