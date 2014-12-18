/*!
 Linphone Web - Web plugin of Linphone an audio/video SIP phone
 Copyright (C) 2012-2014 Belledonne Communications

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
 - Ghislain MARY <ghislain.mary@belledonne-communications.com>

 */

#include "contentapi.h"
#include "factoryapi.h"
#include <ortp/b64.h>

namespace LinphoneWeb {

ContentAPI::ContentAPI(LinphoneContent *content) :
		WrapperAPI(APIDescription(this)), mContent(content) {
	FBLOG_DEBUG("ContentAPI::ContentAPI", "this=" << this << "\t" << "content=" << content);
	linphone_content_ref(mContent);
	linphone_content_set_user_data(mContent, this);
}

ContentAPI::ContentAPI(const LinphoneContent *content) :
		WrapperAPI(APIDescription(this)), mContent(const_cast<LinphoneContent *>(content)) {
	linphone_content_ref(mContent);
	linphone_content_set_user_data(mContent, this);
}

ContentAPI::~ContentAPI() {
	FBLOG_DEBUG("ContentAPI::~ContentAPI", "this=" << this);
	if (mContent != NULL) {
		linphone_content_set_user_data(mContent, NULL);
		linphone_content_unref(mContent);
	}
}

void ContentAPI::initProxy() {
	registerProperty("buffer", make_property(this, &ContentAPI::getBuffer, &ContentAPI::setBuffer));
	registerProperty("encoding", make_property(this, &ContentAPI::getEncoding, &ContentAPI::setEncoding));
	registerProperty("name", make_property(this, &ContentAPI::getName, &ContentAPI::setName));
	registerProperty("size", make_property(this, &ContentAPI::getSize, &ContentAPI::setSize));
	registerProperty("stringBuffer", make_property(this, &ContentAPI::getStringBuffer, &ContentAPI::setStringBuffer));
	registerProperty("subtype", make_property(this, &ContentAPI::getSubtype, &ContentAPI::setSubtype));
	registerProperty("type", make_property(this, &ContentAPI::getType, &ContentAPI::setType));
}

StringPtr ContentAPI::getBuffer() const {
	CORE_MUTEX
	FBLOG_DEBUG("ContentAPI::getBuffer", "this=" << this);
	size_t b64Size = b64::b64_encode(NULL, linphone_content_get_size(mContent), NULL, 0);
	char *b64Buffer = (char *)ms_malloc(b64Size);
	b64::b64_encode(linphone_content_get_buffer(mContent), linphone_content_get_size(mContent), b64Buffer, b64Size);
	StringPtr b64String = CHARPTR_TO_STRING(b64Buffer);
	ms_free(b64Buffer);
	return b64String;
}

void ContentAPI::setBuffer(StringPtr const &b64String) {
	CORE_MUTEX
	size_t b64Size = b64String ? b64String->length() : 0;
	FBLOG_DEBUG("ContentAPI::setBuffer", "this=" << this << "\t" << "b64String.length()=" << b64Size);
	const char *b64Buffer = STRING_TO_CHARPTR(b64String);
	size_t bufferSize = b64::b64_decode(b64Buffer, b64Size, NULL, 0);
	uint8_t *buffer = (uint8_t *)ms_malloc(bufferSize);
	bufferSize = b64::b64_decode(b64Buffer, b64Size, buffer, bufferSize);
	linphone_content_set_buffer(mContent, &buffer[0], bufferSize);
	ms_free(buffer);
}

StringPtr ContentAPI::getEncoding() const {
	CORE_MUTEX
	FBLOG_DEBUG("ContentAPI::getEncoding", "this=" << this);
	return CHARPTR_TO_STRING(linphone_content_get_encoding(mContent));
}

void ContentAPI::setEncoding(StringPtr const &encoding) {
	CORE_MUTEX
	FBLOG_DEBUG("ContentAPI::setEncoding", "this=" << this << "\t" << "encoding=" << encoding);
	linphone_content_set_encoding(mContent, STRING_TO_CHARPTR(encoding));
}

StringPtr ContentAPI::getName() const {
	CORE_MUTEX
	FBLOG_DEBUG("ContentAPI::getName", "this=" << this);
	return CHARPTR_TO_STRING(linphone_content_get_name(mContent));
}

void ContentAPI::setName(StringPtr const &name) {
	CORE_MUTEX
	FBLOG_DEBUG("ContentAPI::setName", "this=" << this << "\t" << "name=" << name);
	linphone_content_set_name(mContent, STRING_TO_CHARPTR(name));
}

size_t ContentAPI::getSize() const {
	CORE_MUTEX
	FBLOG_DEBUG("ContentAPI::getSize", "this=" << this);
	return linphone_content_get_size(mContent);
}

void ContentAPI::setSize(size_t size) {
	CORE_MUTEX
	FBLOG_DEBUG("ContentAPI::setSize", "this" << this << "\t" << "size=" << size);
	linphone_content_set_size(mContent, size);
}

StringPtr ContentAPI::getStringBuffer() const {
	CORE_MUTEX
	FBLOG_DEBUG("ContentAPI::getStringBuffer", "this=" << this);
	return CHARPTR_TO_STRING(linphone_content_get_string_buffer(mContent));
}

void ContentAPI::setStringBuffer(StringPtr const &buffer) {
	CORE_MUTEX
	FBLOG_DEBUG("ContentAPI::setStringBuffer", "this=" << this << "\t" << "buffer=" << buffer);
	linphone_content_set_string_buffer(mContent, STRING_TO_CHARPTR(buffer));
}

StringPtr ContentAPI::getSubtype() const {
	CORE_MUTEX
	FBLOG_DEBUG("ContentAPI::getSubtype", "this=" << this);
	return CHARPTR_TO_STRING(linphone_content_get_subtype(mContent));
}

void ContentAPI::setSubtype(StringPtr const &subtype) {
	CORE_MUTEX
	FBLOG_DEBUG("ContentAPI::setSubtype", "this=" << this << "\t" << "subtype=" << subtype);
	linphone_content_set_subtype(mContent, STRING_TO_CHARPTR(subtype));
}

StringPtr ContentAPI::getType() const {
	CORE_MUTEX
	FBLOG_DEBUG("ContentAPI::getType", "this=" << this);
	return CHARPTR_TO_STRING(linphone_content_get_type(mContent));
}

void ContentAPI::setType(StringPtr const &type) {
	CORE_MUTEX
	FBLOG_DEBUG("ContentAPI::setType", "this=" << this << "\t" << "type=" << type);
	linphone_content_set_type(mContent, STRING_TO_CHARPTR(type));
}

} // LinphoneWeb
