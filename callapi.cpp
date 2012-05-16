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

#include "callapi.h"

CallAPI::CallAPI(LinphoneCall *call) :
		mCall(call) {
	FBLOG_DEBUG("CallAPI::CallAPI", this);
	linphone_call_ref(mCall);
	linphone_call_set_user_pointer(mCall, this);

	registerMethod("getState", FB::make_method(this, &CallAPI::getState));
	registerMethod("getRemoteAddress", FB::make_method(this, &CallAPI::getRemoteAddress));
	registerMethod("getDir", FB::make_method(this, &CallAPI::getDir));
}

CallAPI::~CallAPI() {
	FBLOG_DEBUG("CallAPI::~CallAPI", this);
	linphone_call_set_user_pointer(mCall, NULL);
	linphone_call_unref(mCall);
}

int CallAPI::getState() const {
	return linphone_call_get_state(mCall);
}

std::string CallAPI::getRemoteAddress() const {
	const char *txt = linphone_call_get_remote_address_as_string(mCall);
	return txt != NULL ? txt : "";
}

int CallAPI::getDir() const {
	return linphone_call_get_dir(mCall);
}

CallAPIPtr CallAPI::get(LinphoneCall *call) {
	void *ptr = linphone_call_get_user_pointer(call);
	CallAPIPtr shared_ptr;
	if (ptr == NULL) {
		shared_ptr = CallAPIPtr(new CallAPI(call));
	} else {
		shared_ptr = boost::static_pointer_cast<CallAPI>(reinterpret_cast<CallAPI *>(ptr)->shared_from_this());
	}
	return shared_ptr;
}
