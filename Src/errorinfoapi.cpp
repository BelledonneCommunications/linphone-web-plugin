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

 */

#include "errorinfoapi.h"
#include "coreapi.h"

#include "utils.h"
#include "factoryapi.h"

namespace LinphoneWeb {

ErrorInfoAPI::ErrorInfoAPI(const LinphoneErrorInfo *ei) :
		WrapperAPI(APIDescription(this)) {
	FBLOG_DEBUG("ErrorInfoAPI::ErrorInfoAPI", "this=" << this << "\t" << "ei=" << ei);
	mReason = linphone_error_info_get_reason(ei);
	mProtocolCode = linphone_error_info_get_protocol_code(ei);
	mPhrase = CHARPTR_TO_STRING(linphone_error_info_get_phrase(ei));
	mDetails = CHARPTR_TO_STRING(linphone_error_info_get_details(ei));
}

void ErrorInfoAPI::initProxy() {
	registerProperty("details", make_property(this, &ErrorInfoAPI::getDetails));
	registerProperty("phrase", make_property(this, &ErrorInfoAPI::getPhrase));
	registerProperty("protocolCode", make_property(this, &ErrorInfoAPI::getProtocolCode));
	registerProperty("reason", make_property(this, &ErrorInfoAPI::getReason));
}

ErrorInfoAPI::~ErrorInfoAPI() {
	FBLOG_DEBUG("ErrorInfoAPI::~ErrorInfoAPI", "this=" << this);
}

int ErrorInfoAPI::getReason() const {
	FBLOG_DEBUG("ErrorInfoAPI::getReason", "this=" << this);
	return mReason;
}

int ErrorInfoAPI::getProtocolCode() const {
	FBLOG_DEBUG("ErrorInfoAPI::getProtocolCode", "this=" << this);
	return mProtocolCode;
}

StringPtr ErrorInfoAPI::getPhrase() const {
	FBLOG_DEBUG("ErrorInfoAPI::getPhrase", "this=" << this);
	return mPhrase;
}

StringPtr ErrorInfoAPI::getDetails() const {
	FBLOG_DEBUG("ErrorInfoAPI::getDetails", "this=" << this);
	return mDetails;
}

} // LinphoneWeb
