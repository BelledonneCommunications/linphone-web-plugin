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
		JSAPIAuto(APIDescription(this)), mCore(core), mPayloadType(payloadType), mConst(false) {
	FBLOG_DEBUG("PayloadTypeAPI::PayloadTypeAPI", "this=" << this << "\t" << "core=" << core << ", payloadType=" << payloadType);
	//mPayloadType->user_data = this;
	initProxy();
}

PayloadTypeAPI::PayloadTypeAPI(const CoreAPIPtr &core, const PayloadType *payloadType) :
		JSAPIAuto(APIDescription(this)), mCore(core), mPayloadType(const_cast<PayloadType *>(payloadType)), mConst(true) {
	FBLOG_DEBUG("PayloadTypeAPI::PayloadTypeAPI", "this=" << this << "\t" << "core=" << core << ", payloadType=" << payloadType);
	//mPayloadType->user_data = this;
	initProxy();
}

void PayloadTypeAPI::initProxy() {
	if (mConst) {
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
	} else {
		registerProperty("type", make_property(this, &PayloadTypeAPI::getType, &PayloadTypeAPI::setType));
		registerProperty("clockRate", make_property(this, &PayloadTypeAPI::getClockRate, &PayloadTypeAPI::setClockRate));
		registerProperty("bitsPerSample", make_property(this, &PayloadTypeAPI::getBitsPerSample, &PayloadTypeAPI::setBitsPerSample));
		registerProperty("zeroPattern", make_property(this, &PayloadTypeAPI::getZeroPattern, &PayloadTypeAPI::setZeroPattern));
		registerProperty("patternLength", make_property(this, &PayloadTypeAPI::getPatternLength, &PayloadTypeAPI::setPatternLength));
		registerProperty("normalBitrate", make_property(this, &PayloadTypeAPI::getNormalBitrate, &PayloadTypeAPI::setNormalBitrate));
		registerProperty("mimeType", make_property(this, &PayloadTypeAPI::getMimeType, &PayloadTypeAPI::setMimeType));
		registerProperty("channels", make_property(this, &PayloadTypeAPI::getChannels, &PayloadTypeAPI::setChannels));
		registerProperty("recvFmtp", make_property(this, &PayloadTypeAPI::getRecvFmtp, &PayloadTypeAPI::setRecvFmtp));
		registerProperty("sendFmtp", make_property(this, &PayloadTypeAPI::getSendFmtp, &PayloadTypeAPI::setSendFmtp));
		registerProperty("flags", make_property(this, &PayloadTypeAPI::getFlags));
	}
	registerProperty("enabled", make_property(this, &PayloadTypeAPI::getEnabled, &PayloadTypeAPI::setEnabled));

	registerMethod("clone", make_method(this, &PayloadTypeAPI::clone));
	registerMethod("getRtpmap", make_method(this, &PayloadTypeAPI::getRtpmap));
}

PayloadTypeAPI::~PayloadTypeAPI() {
	FBLOG_DEBUG("ProxyConfigAPI::~ProxyConfigAPI", "this=" << this);
	mPayloadType->user_data = NULL;
}

int PayloadTypeAPI::getType() const {
	FBLOG_DEBUG("PayloadTypeAPI::getType()", "this=" << this);
	return mPayloadType->type;
}
void PayloadTypeAPI::setType(int type) {
	FBLOG_DEBUG("PayloadTypeAPI::setType()", "this=" << this << "\t" << "type=" << type);
	mPayloadType->type = type;
}

int PayloadTypeAPI::getClockRate() const {
	FBLOG_DEBUG("PayloadTypeAPI::getClockRate()", "this=" << this);
	return mPayloadType->clock_rate;
}
void PayloadTypeAPI::setClockRate(int rate) {
	FBLOG_DEBUG("PayloadTypeAPI::setClockRate()", "this=" << this << "\t" << "rate=" << rate);
	mPayloadType->clock_rate = rate;
}

int PayloadTypeAPI::getBitsPerSample() const {
	FBLOG_DEBUG("PayloadTypeAPI::getBitsPerSample()", "this=" << this);
	return mPayloadType->bits_per_sample;
}
void PayloadTypeAPI::setBitsPerSample(int bps) {
	FBLOG_DEBUG("PayloadTypeAPI::setBitsPerSample()", "this=" << this << "\t" << "bps=" << bps);
	mPayloadType->bits_per_sample = bps;
}

std::string PayloadTypeAPI::getZeroPattern() const {
	FBLOG_DEBUG("PayloadTypeAPI::getZeroPattern()", "this=" << this);
	return CHARPTR_TO_STRING(mPayloadType->zero_pattern);
}
void PayloadTypeAPI::setZeroPattern(const std::string &pattern) {
	FBLOG_DEBUG("PayloadTypeAPI::setZeroPattern()", "this=" << this << "\t" << "pattern=" << pattern);
	if (mPayloadType->zero_pattern != NULL)
		free(mPayloadType->zero_pattern);
	mPayloadType->zero_pattern = strdup(pattern.c_str());
}

int PayloadTypeAPI::getPatternLength() const {
	FBLOG_DEBUG("PayloadTypeAPI::getPatternLength()", "this=" << this);
	return mPayloadType->pattern_length;
}
void PayloadTypeAPI::setPatternLength(int length) {
	FBLOG_DEBUG("PayloadTypeAPI::setPatternLength()", "this=" << this << "\t" << "length=" << length);
	mPayloadType->pattern_length = length;
}

int PayloadTypeAPI::getNormalBitrate() const {
	FBLOG_DEBUG("PayloadTypeAPI::getNormalBitrate()", "this=" << this);
	return mPayloadType->normal_bitrate;
}
void PayloadTypeAPI::setNormalBitrate(int rate) {
	FBLOG_DEBUG("PayloadTypeAPI::setNormalBitrate()", "this=" << this << "\t" << "rate=" << rate);
	mPayloadType->normal_bitrate = rate;
}

std::string PayloadTypeAPI::getMimeType() const {
	FBLOG_DEBUG("PayloadTypeAPI::getMimeType()", "this=" << this);
	return CHARPTR_TO_STRING(mPayloadType->mime_type);
}
void PayloadTypeAPI::setMimeType(const std::string &mime) {
	FBLOG_DEBUG("PayloadTypeAPI::setMimeType()", "this=" << this << "\t" << "mime=" << mime);
	if (mPayloadType->mime_type != NULL)
		free(mPayloadType->mime_type);
	mPayloadType->mime_type = strdup(mime.c_str());
}

int PayloadTypeAPI::getChannels() const {
	FBLOG_DEBUG("PayloadTypeAPI::getChannels()", "this=" << this);
	return mPayloadType->channels;
}
void PayloadTypeAPI::setChannels(int channels) {
	FBLOG_DEBUG("PayloadTypeAPI::setNormalBitrate()", "this=" << this << "\t" << "channels=" << channels);
	mPayloadType->channels = channels;
}

std::string PayloadTypeAPI::getRecvFmtp() const {
	FBLOG_DEBUG("PayloadTypeAPI::getRecvFmtp()", "this=" << this);
	return CHARPTR_TO_STRING(mPayloadType->recv_fmtp);
}
void PayloadTypeAPI::setRecvFmtp(const std::string &rfmtp) {
	FBLOG_DEBUG("PayloadTypeAPI::setRecvFmtp()", "this=" << this << "\t" << "rfmtp=" << rfmtp);
	if (mPayloadType->recv_fmtp != NULL)
		free(mPayloadType->recv_fmtp);
	mPayloadType->recv_fmtp = strdup(rfmtp.c_str());
}

std::string PayloadTypeAPI::getSendFmtp() const {
	FBLOG_DEBUG("PayloadTypeAPI::getSendFmtp()", "this=" << this);
	return CHARPTR_TO_STRING(mPayloadType->send_fmtp);
}
void PayloadTypeAPI::setSendFmtp(const std::string &sfmtp) {
	FBLOG_DEBUG("PayloadTypeAPI::setSendFmtp()", "this=" << this << "\t" << "sfmtp=" << sfmtp);
	if (mPayloadType->send_fmtp != NULL)
		free(mPayloadType->send_fmtp);
	mPayloadType->send_fmtp = strdup(sfmtp.c_str());
}

int PayloadTypeAPI::getFlags() const {
	FBLOG_DEBUG("PayloadTypeAPI::getFlags()", "this=" << this);
	return mPayloadType->flags;
}
void PayloadTypeAPI::setFlags(int flags) {
	FBLOG_DEBUG("PayloadTypeAPI::setFlags()", "this=" << this << "\t" << "flags=" << flags);
	mPayloadType->flags = flags;
}

bool PayloadTypeAPI::getEnabled() const {
	FBLOG_DEBUG("PayloadTypeAPI::getEnabled()", "this=" << this);
	CoreAPIPtr core(mCore.lock());
	if (core != NULL) {
		return linphone_core_payload_type_enabled(core->getRef(), mPayloadType) == TRUE ? true : false;
	}
	return false;
}

void PayloadTypeAPI::setEnabled(bool enable) {
	FBLOG_DEBUG("PayloadTypeAPI::setEnabled()", "this=" << this << "\t" << "enable=" << enable);
	CoreAPIPtr core(mCore.lock());
	if (core != NULL) {
		linphone_core_enable_payload_type(core->getRef(), mPayloadType, enable ? TRUE : FALSE);
	}
}

PayloadTypeAPIPtr PayloadTypeAPI::clone() const {
	return PayloadTypeAPI::get(payload_type_clone(mPayloadType));
}

std::string PayloadTypeAPI::getRtpmap() const {
	return CHARPTR_TO_STRING(payload_type_get_rtpmap(mPayloadType));
}

PayloadTypeAPIPtr PayloadTypeAPI::get(const CoreAPIPtr &core, PayloadType *payloadType) {
	if (payloadType == NULL)
		return PayloadTypeAPIPtr();

	//void *ptr = payloadType->user_data;
	PayloadTypeAPIPtr shared_ptr;
	/*if (ptr == NULL) {*/
	shared_ptr = PayloadTypeAPIPtr(new PayloadTypeAPI(core, payloadType));
	/*} else {
	 shared_ptr = boost::static_pointer_cast<PayloadTypeAPI>(reinterpret_cast<PayloadTypeAPI *>(ptr)->shared_from_this());
	 }*/
	return shared_ptr;
}

PayloadTypeAPIPtr PayloadTypeAPI::get(const CoreAPIPtr &core, const PayloadType *payloadType) {
	if (payloadType == NULL)
		return PayloadTypeAPIPtr();

	//void *ptr = payloadType->user_data;
	PayloadTypeAPIPtr shared_ptr;
	/*if (ptr == NULL) {*/
	shared_ptr = PayloadTypeAPIPtr(new PayloadTypeAPI(core, payloadType));
	/*} else {
	 shared_ptr = boost::static_pointer_cast<PayloadTypeAPI>(reinterpret_cast<PayloadTypeAPI *>(ptr)->shared_from_this());
	 }*/
	return shared_ptr;
}
