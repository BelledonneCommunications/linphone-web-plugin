/*!
 Linphone Web - Web plugin of Linphone an audio/video SIP phone
 Copyright (C) 2012-2013 Belledonne Communications
 
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
 
 
 Authors:
 - Yann Diorcet <diorcet.yann@gmail.com>
 
 */

#ifndef H_PAYLOADTYPEAPI
#define H_PAYLOADTYPEAPI

#include <linphone/linphonecore.h>
#include "wrapperapi.h"

namespace LinphoneWeb {

FB_FORWARD_PTR(CoreAPI)

FB_FORWARD_PTR(PayloadTypeAPI)
class PayloadTypeAPI: public WrapperAPI {
	friend class FactoryAPI;
private:
	PayloadType *mPayloadType;

	PayloadTypeAPI(PayloadType *payloadType);
	PayloadTypeAPI(const PayloadType *payloadType);
	
protected:
	virtual void initProxy();

public:
	virtual ~PayloadTypeAPI();

	PayloadTypeAPIPtr clone() const;
	StringPtr getRtpmap() const;

	int getType() const;
	void setType(int type);

	int getClockRate() const;
	void setClockRate(int rate);

	int getBitsPerSample() const;
	void setBitsPerSample(int bps);

	int getNormalBitrate() const;
	void setNormalBitrate(int bitrate);

	StringPtr getMimeType() const;
	void setMimeType(StringPtr const &mime);

	int getChannels() const;
	void setChannels(int channels);

	StringPtr getRecvFmtp() const;
	void setRecvFmtp(StringPtr const &rfmtp);

	StringPtr getSendFmtp() const;
	void setSendFmtp(StringPtr const &sfmtp);

	int getFlags() const;
	void setFlags(int flags);

	inline PayloadType *getRef() const {
		return mPayloadType;
	}
};
	
} // LinphoneWeb

#endif //H_PAYLOADTYPEAPI
