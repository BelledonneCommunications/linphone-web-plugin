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

#include "calllogapi.h"
#include "utils.h"

CallLogAPI::CallLogAPI(LinphoneCallLog *callLog) :
		JSAPIAuto(APIDescription(this)), mCallLog(callLog) {
    mUsed = true;
    mConst = false;
	FBLOG_DEBUG("CallLogAPI::CallLogAPI", "this=" << this << "\t" << "callLog=" << callLog);
	linphone_call_log_set_user_pointer(mCallLog, this);
	initProxy();
}

void CallLogAPI::initProxy() {
	registerMethod("toStr", make_method(this, &CallLogAPI::toStr));
	registerProperty("refKey", make_property(this, &CallLogAPI::getRefKey));
}

CallLogAPI::~CallLogAPI() {
	FBLOG_DEBUG("CallLogAPI::~CallLogAPI", "this=" << this);
	linphone_call_log_set_user_pointer(mCallLog, NULL);
}

std::string CallLogAPI::getRefKey() const {
	FBLOG_DEBUG("CallLogAPI::getRefKey", "this=" << this);
	return CHARPTR_TO_STRING(linphone_call_log_get_ref_key(mCallLog));
}

void CallLogAPI::setRefKey(const std::string &refKey) {
	FBLOG_DEBUG("CallLogAPI::setRefKey", "this=" << this << "\t" << "refKey=" << refKey);
	linphone_call_log_set_ref_key(mCallLog, refKey.c_str());
}

std::string CallLogAPI::toStr() const {
	FBLOG_DEBUG("CallLogAPI::toStr", "this=" << this);
	return CHARPTR_TO_STRING(linphone_call_log_to_str(mCallLog));
}
