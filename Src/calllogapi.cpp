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

#include "calllogapi.h"

#include "utils.h"
#include "factoryapi.h"

namespace LinphoneWeb {

CallLogAPI::CallLogAPI(LinphoneCallLog *callLog) :
		WrapperAPI(APIDescription(this)), mCallLog(callLog) {
	FBLOG_DEBUG("CallLogAPI::CallLogAPI", "this=" << this << "\t" << "callLog=" << callLog);
	linphone_call_log_set_user_pointer(mCallLog, this);
}

void CallLogAPI::initProxy() {
	registerMethod("toStr", make_method(this, &CallLogAPI::toStr));
	
	registerProperty("callId", make_property(this, &CallLogAPI::getCallId));
	registerProperty("dir", make_property(this, &CallLogAPI::getDir));
	registerProperty("duration", make_property(this, &CallLogAPI::getDuration));
	registerProperty("from", make_property(this, &CallLogAPI::getFrom));
	registerProperty("quality", make_property(this, &CallLogAPI::getQuality));
	registerProperty("remoteAddress", make_property(this, &CallLogAPI::getRemoteAddress));
	registerProperty("startDate", make_property(this, &CallLogAPI::getStartDate));
	registerProperty("status", make_property(this, &CallLogAPI::getStatus));
	registerProperty("to", make_property(this, &CallLogAPI::getTo));
	registerProperty("videoEnabled", make_property(this, &CallLogAPI::videoEnabled));
}

CallLogAPI::~CallLogAPI() {
	FBLOG_DEBUG("CallLogAPI::~CallLogAPI", "this=" << this);
	linphone_call_log_set_user_pointer(mCallLog, NULL);
}

StringPtr CallLogAPI::getCallId() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallLogAPI::getCallId", "this=" << this);
	return CHARPTR_TO_STRING(linphone_call_log_get_call_id(mCallLog));
}

LinphoneCallDir CallLogAPI::getDir() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallLogAPI::getDir", "this=" << this);
	return linphone_call_log_get_dir(mCallLog);
}

int CallLogAPI::getDuration() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallLogAPI::getDuration", "this=" << this);
	return linphone_call_log_get_duration(mCallLog);
}

AddressAPIPtr CallLogAPI::getFrom() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallLogAPI::getFrom", "this=" << this);
	return getFactory()->getAddress(linphone_call_log_get_from(mCallLog));
}

float CallLogAPI::getQuality() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallLogAPI::getQuality", "this=" << this);
	return linphone_call_log_get_quality(mCallLog);
}

AddressAPIPtr CallLogAPI::getRemoteAddress() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallLogAPI::getRemoteAddress", "this=" << this);
	return getFactory()->getAddress(linphone_call_log_get_remote_address(mCallLog));
}

time_t CallLogAPI::getStartDate() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallLogAPI::getStartDate", "this=" << this);
	// WARNING: Keep only the least-significant 32 bits because of difference of definition of time_t between mingw and visual studio
	return (linphone_call_log_get_start_date(mCallLog) & 0xFFFFFFFF);
}

LinphoneCallStatus CallLogAPI::getStatus() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallLogAPI::getStatus", "this=" << this);
	return linphone_call_log_get_status(mCallLog);
}

AddressAPIPtr CallLogAPI::getTo() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallLogAPI::getTo", "this=" << this);
	return getFactory()->getAddress(linphone_call_log_get_to(mCallLog));
}

bool CallLogAPI::videoEnabled() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("CallLogAPI::videoEnabled", "this=" << this);
	return linphone_call_log_video_enabled(mCallLog) == TRUE? true:false;
}

StringPtr CallLogAPI::toStr() const {
	CORE_MUTEX

	FBLOG_DEBUG("CallLogAPI::toStr", "this=" << this);
	return CHARPTR_TO_STRING(linphone_call_log_to_str(mCallLog));
}
	
} // LinphoneWeb
