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

#include "payloadtypeapi.h"
#include "coreapi.h"
#include "utils.h"

PayloadTypeAPI::PayloadTypeAPI(const CoreAPIPtr &core, PayloadType *payloadType) :
		JSAPIAuto(APIDescription(this)), mCore(core), mPayloadType(payloadType) {
	FBLOG_DEBUG("PayloadTypeAPI::PayloadTypeAPI", this);
	//mPayloadType->user_data = this;

	registerProperty("type", make_property(this, &PayloadTypeAPI::getType));
	registerProperty("clockRate", make_property(this, &PayloadTypeAPI::getClockRate));
	registerProperty("bitsPerSample", make_property(this, &PayloadTypeAPI::getBitsPerSample));
	registerProperty("zeroPattern", make_property(this, &PayloadTypeAPI::getZeroPattern));
	registerProperty("patternLength", make_property(this, &PayloadTypeAPI::getPatternLength));
	registerProperty("normalBitrate", make_property(this, &PayloadTypeAPI::getNormalBitrate));
	registerProperty("mimeType", make_property(this, &PayloadTypeAPI::getMimeType));
	registerProperty("channels", make_property(this, &PayloadTypeAPI::getChannels));
	registerProperty("recvFmtp", make_property(this, &PayloadTypeAPI::getRecvFmtp));
	registerProperty("sendFmtp", make_property(this, &PayloadTypeAPI::getSendFmtp));
	registerProperty("flags", make_property(this, &PayloadTypeAPI::getFlags));
	registerProperty("enabled", make_property(this, &PayloadTypeAPI::getEnabled, &PayloadTypeAPI::setEnabled));
}

bool PayloadTypeAPI::getEnabled() const {
	FBLOG_DEBUG("PayloadTypeAPI::getEnabled()", this);
	CoreAPIPtr core(mCore.lock());
	if (core != NULL) {
		return linphone_core_payload_type_enabled(core->getRef(), mPayloadType) == TRUE ? true : false;
	}
	return false;
}

void PayloadTypeAPI::setEnabled(bool enable) {
	FBLOG_DEBUG("PayloadTypeAPI::setEnabled()", "enable=" << enable);
	CoreAPIPtr core(mCore.lock());
	if (core != NULL) {
		linphone_core_enable_payload_type(core->getRef(), mPayloadType, enable ? TRUE : FALSE);
	}
}

PayloadTypeAPI::~PayloadTypeAPI() {
	FBLOG_DEBUG("ProxyConfigAPI::~ProxyConfigAPI", this);
	mPayloadType->user_data = NULL;
}

PayloadTypeAPIPtr PayloadTypeAPI::get(const CoreAPIPtr &core, PayloadType *payloadType) {
	//void *ptr = payloadType->user_data;
	PayloadTypeAPIPtr shared_ptr;
	/*if (ptr == NULL) {*/
	shared_ptr = PayloadTypeAPIPtr(new PayloadTypeAPI(core, payloadType));
	/*} else {
	 shared_ptr = boost::static_pointer_cast<PayloadTypeAPI>(reinterpret_cast<PayloadTypeAPI *>(ptr)->shared_from_this());
	 }*/
	return shared_ptr;
}
