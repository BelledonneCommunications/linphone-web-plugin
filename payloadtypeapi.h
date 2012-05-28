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
	CoreAPIWeakPtr mCore;
	PayloadType *mPayloadType;

	PayloadTypeAPI(const CoreAPIPtr &core, PayloadType *payloadType);
	void initProxy();
public:
	~PayloadTypeAPI();

	int getType() const;
	int getClockRate() const;
	int getBitsPerSample() const;
	std::string getZeroPattern() const;
	int getPatternLength() const;
	int getNormalBitrate() const;
	std::string getMimeType() const;
	int getChannels() const;
	std::string getRecvFmtp() const;
	std::string getSendFmtp() const;
	int getFlags() const;
	bool getEnabled() const;
	void setEnabled(bool enable);

	inline PayloadType *getRef() const {
		return mPayloadType;
	}
	static PayloadTypeAPIPtr get(const CoreAPIPtr &core, PayloadType *payloadType);
};

#endif //H_PAYLOADTYPEAPI
