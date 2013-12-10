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

#include "payloadtypeapi.h"
#include "coreapi.h"

#include "utils.h"
#include "factoryapi.h"

namespace LinphoneWeb {

PayloadTypeAPI::PayloadTypeAPI(PayloadType *payloadType) :
		WrapperAPI(APIDescription(this)), mPayloadType(payloadType) {
	FBLOG_DEBUG("PayloadTypeAPI::PayloadTypeAPI", "this=" << this << ", payloadType=" << payloadType);
	//mPayloadType->user_data = this;
}

PayloadTypeAPI::PayloadTypeAPI(const PayloadType *payloadType) :
		WrapperAPI(APIDescription(this)), mPayloadType(const_cast<PayloadType *>(payloadType)) {
	FBLOG_DEBUG("PayloadTypeAPI::PayloadTypeAPI", "this=" << this << ", payloadType=" << payloadType);
	//mPayloadType->user_data = this;
}

void PayloadTypeAPI::initProxy() {
	if (isConst()) {
		registerProperty("type", make_property(this, &PayloadTypeAPI::getType));
		registerProperty("clockRate", make_property(this, &PayloadTypeAPI::getClockRate));
		registerProperty("bitsPerSample", make_property(this, &PayloadTypeAPI::getBitsPerSample));
		registerProperty("normalBitrate", make_property(this, &PayloadTypeAPI::getNormalBitrate));
		registerProperty("mimeType", make_property(this, &PayloadTypeAPI::getMimeType));
		registerProperty("channels", make_property(this, &PayloadTypeAPI::getChannels));
		registerProperty("recvFmtp", make_property(this, &PayloadTypeAPI::getRecvFmtp));
		registerProperty("sendFmtp", make_property(this, &PayloadTypeAPI::getSendFmtp));
		registerProperty("flags", make_property(this, &PayloadTypeAPI::getFlags));
	} else {
		registerProperty("type", make_property(this, &PayloadTypeAPI::getType, &PayloadTypeAPI::setType));
		registerProperty("clockRate", make_property(this, &PayloadTypeAPI::getClockRate, &PayloadTypeAPI::setClockRate));
		registerProperty("bitsPerSample", make_property(this, &PayloadTypeAPI::getBitsPerSample, &PayloadTypeAPI::setBitsPerSample));
		registerProperty("normalBitrate", make_property(this, &PayloadTypeAPI::getNormalBitrate, &PayloadTypeAPI::setNormalBitrate));
		registerProperty("mimeType", make_property(this, &PayloadTypeAPI::getMimeType, &PayloadTypeAPI::setMimeType));
		registerProperty("channels", make_property(this, &PayloadTypeAPI::getChannels, &PayloadTypeAPI::setChannels));
		registerProperty("recvFmtp", make_property(this, &PayloadTypeAPI::getRecvFmtp, &PayloadTypeAPI::setRecvFmtp));
		registerProperty("sendFmtp", make_property(this, &PayloadTypeAPI::getSendFmtp, &PayloadTypeAPI::setSendFmtp));
		registerProperty("flags", make_property(this, &PayloadTypeAPI::getFlags));
	}

	registerMethod("clone", make_method(this, &PayloadTypeAPI::clone));
	registerMethod("getRtpmap", make_method(this, &PayloadTypeAPI::getRtpmap));
}

PayloadTypeAPI::~PayloadTypeAPI() {
	FBLOG_DEBUG("ProxyConfigAPI::~ProxyConfigAPI", "this=" << this);
	//mPayloadType->user_data = NULL;
}

int PayloadTypeAPI::getType() const {
	CORE_MUTEX

	FBLOG_DEBUG("PayloadTypeAPI::getType", "this=" << this);
	return mPayloadType->type;
}
void PayloadTypeAPI::setType(int type) {
	CORE_MUTEX

	FBLOG_DEBUG("PayloadTypeAPI::setType", "this=" << this << "\t" << "type=" << type);
	mPayloadType->type = type;
}

int PayloadTypeAPI::getClockRate() const {
	CORE_MUTEX

	FBLOG_DEBUG("PayloadTypeAPI::getClockRate", "this=" << this);
	return mPayloadType->clock_rate;
}
void PayloadTypeAPI::setClockRate(int rate) {
	CORE_MUTEX

	FBLOG_DEBUG("PayloadTypeAPI::setClockRate", "this=" << this << "\t" << "rate=" << rate);
	mPayloadType->clock_rate = rate;
}

int PayloadTypeAPI::getBitsPerSample() const {
	CORE_MUTEX

	FBLOG_DEBUG("PayloadTypeAPI::getBitsPerSample", "this=" << this);
	return mPayloadType->bits_per_sample;
}
void PayloadTypeAPI::setBitsPerSample(int bps) {
	CORE_MUTEX

	FBLOG_DEBUG("PayloadTypeAPI::setBitsPerSample", "this=" << this << "\t" << "bps=" << bps);
	mPayloadType->bits_per_sample = bps;
}

int PayloadTypeAPI::getNormalBitrate() const {
	CORE_MUTEX

	FBLOG_DEBUG("PayloadTypeAPI::getNormalBitrate", "this=" << this);
	return mPayloadType->normal_bitrate;
}
void PayloadTypeAPI::setNormalBitrate(int rate) {
	CORE_MUTEX

	FBLOG_DEBUG("PayloadTypeAPI::setNormalBitrate", "this=" << this << "\t" << "rate=" << rate);
	mPayloadType->normal_bitrate = rate;
}

StringPtr PayloadTypeAPI::getMimeType() const {
	CORE_MUTEX

	FBLOG_DEBUG("PayloadTypeAPI::getMimeType", "this=" << this);
	return CHARPTR_TO_STRING(mPayloadType->mime_type);
}
void PayloadTypeAPI::setMimeType(StringPtr const &mime) {
	CORE_MUTEX

	FBLOG_DEBUG("PayloadTypeAPI::setMimeType", "this=" << this << "\t" << "mime=" << mime);
	if (mPayloadType->mime_type != NULL)
		free(mPayloadType->mime_type);
	mPayloadType->mime_type = strdup(STRING_TO_CHARPTR(mime));
}

int PayloadTypeAPI::getChannels() const {
	CORE_MUTEX

	FBLOG_DEBUG("PayloadTypeAPI::getChannels", "this=" << this);
	return mPayloadType->channels;
}
void PayloadTypeAPI::setChannels(int channels) {
	CORE_MUTEX

	FBLOG_DEBUG("PayloadTypeAPI::setNormalBitrate", "this=" << this << "\t" << "channels=" << channels);
	mPayloadType->channels = channels;
}

StringPtr PayloadTypeAPI::getRecvFmtp() const {
	CORE_MUTEX

	FBLOG_DEBUG("PayloadTypeAPI::getRecvFmtp", "this=" << this);
	return CHARPTR_TO_STRING(mPayloadType->recv_fmtp);
}

void PayloadTypeAPI::setRecvFmtp(StringPtr const &rfmtp) {
	CORE_MUTEX

	FBLOG_DEBUG("PayloadTypeAPI::setRecvFmtp", "this=" << this << "\t" << "rfmtp=" << rfmtp);
	if (mPayloadType->recv_fmtp != NULL)
		free(mPayloadType->recv_fmtp);
	mPayloadType->recv_fmtp = strdup(STRING_TO_CHARPTR(rfmtp));
}

StringPtr PayloadTypeAPI::getSendFmtp() const {
	CORE_MUTEX

	FBLOG_DEBUG("PayloadTypeAPI::getSendFmtp", "this=" << this);
	return CHARPTR_TO_STRING(mPayloadType->send_fmtp);
}
void PayloadTypeAPI::setSendFmtp(StringPtr const &sfmtp) {
	CORE_MUTEX

	FBLOG_DEBUG("PayloadTypeAPI::setSendFmtp", "this=" << this << "\t" << "sfmtp=" << sfmtp);
	if (mPayloadType->send_fmtp != NULL)
		free(mPayloadType->send_fmtp);
	mPayloadType->send_fmtp = strdup(STRING_TO_CHARPTR(sfmtp));
}

int PayloadTypeAPI::getFlags() const {
	CORE_MUTEX

	FBLOG_DEBUG("PayloadTypeAPI::getFlags", "this=" << this);
	return mPayloadType->flags;
}
void PayloadTypeAPI::setFlags(int flags) {
	CORE_MUTEX

	FBLOG_DEBUG("PayloadTypeAPI::setFlags", "this=" << this << "\t" << "flags=" << flags);
	mPayloadType->flags = flags;
}

PayloadTypeAPIPtr PayloadTypeAPI::clone() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("PayloadTypeAPI::clone", "this=" << this);
	PayloadTypeAPIPtr ret = getFactory()->getPayloadType(payload_type_clone(mPayloadType));
	ret->own();
	return ret;
}

StringPtr PayloadTypeAPI::getRtpmap() const {
	CORE_MUTEX

	FBLOG_DEBUG("PayloadTypeAPI::getRtpmap", "this=" << this);
	return CHARPTR_TO_STRING(payload_type_get_rtpmap(mPayloadType));
}

} // LinphoneWeb
