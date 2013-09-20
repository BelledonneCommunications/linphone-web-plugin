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
 - Ghislain MARY <ghislain.mary@belledonne-communications.com>

 */

#include "friendapi.h"

#include "utils.h"
#include "factoryapi.h"

namespace LinphoneWeb {

FriendAPI::FriendAPI() :
		WrapperAPI(APIDescription(this)) {
	FBLOG_DEBUG("FriendAPI::FriendAPI", "this=" << this);
	mFriend = linphone_friend_new();
}

FriendAPI::FriendAPI(LinphoneFriend *lFriend) :
		WrapperAPI(APIDescription(this)), mFriend(lFriend) {
	FBLOG_DEBUG("FriendAPI::FriendAPI", "this=" << this << "\t" << "lFriend=" << lFriend);
}

FriendAPI::FriendAPI(StringPtr const &address) :
		WrapperAPI(APIDescription(this)) {
	FBLOG_DEBUG("FriendAPI::FriendAPI", "this=" << this << "\t" << "address=" << address);
	mFriend = linphone_friend_new_with_address(STRING_TO_CHARPTR(address));
	if (mFriend == NULL) {
		throw std::invalid_argument("one/many parameters is/are invalid");
	}
}

void FriendAPI::initProxy() {
	registerProperty("address", make_property(this, &FriendAPI::getAddress, &FriendAPI::setAddress));
	registerProperty("incSubscribePolicy", make_property(this, &FriendAPI::getIncSubscribePolicy, &FriendAPI::setIncSubscribePolicy));
	registerProperty("name", make_property(this, &FriendAPI::getName, &FriendAPI::setName));
	registerProperty("presenceModel", make_property(this, &FriendAPI::getPresenceModel));
	registerProperty("refKey", make_property(this, &FriendAPI::getRefKey, &FriendAPI::setRefKey));
	registerProperty("subscribesEnabled", make_property(this, &FriendAPI::subscribesEnabled, &FriendAPI::enableSubscribes));

	registerMethod("done", make_method(this, &FriendAPI::done));
	registerMethod("edit", make_method(this, &FriendAPI::edit));
	registerMethod("inList", make_method(this, &FriendAPI::inList));
}

FriendAPI::~FriendAPI() {
	FBLOG_DEBUG("FriendAPI::~FriendAPI", "this=" << this);
	if (isOwned()) {
		if(mFriend != NULL) {
			linphone_friend_destroy(mFriend);
		}
	}
}

void FriendAPI::setAddress(AddressAPIPtr const &address) {
	CORE_MUTEX

	FBLOG_DEBUG("FriendAPI::setAddress", "this=" << this << "\t" << "address=" << address);
	linphone_friend_set_address(mFriend, address->getRef());
}

AddressAPIPtr FriendAPI::getAddress() const {
	CORE_MUTEX

	FBLOG_DEBUG("FriendAPI::getAddress", "this=" << this);
	const LinphoneAddress *address = linphone_friend_get_address(mFriend);
	return getFactory()->getAddress(address);
}

void FriendAPI::setName(StringPtr const &name) {
	CORE_MUTEX

	FBLOG_DEBUG("FriendAPI::setName", "this=" << this << "\t" << "name=" << name);
	linphone_friend_set_name(mFriend, STRING_TO_CHARPTR(name));
}

StringPtr FriendAPI::getName() const {
	CORE_MUTEX

	FBLOG_DEBUG("FriendAPI::getName", "this=" << this);
	return CHARPTR_TO_STRING(linphone_friend_get_name(mFriend));
}

void FriendAPI::enableSubscribes(bool enable) {
	CORE_MUTEX

	FBLOG_DEBUG("FriendAPI::enableSubscribes", "this=" << this << "\t" << "enable=" << enable);
	linphone_friend_enable_subscribes(mFriend, enable ? TRUE : FALSE);
}

bool FriendAPI::subscribesEnabled() const {
	CORE_MUTEX

	FBLOG_DEBUG("FriendAPI::subscribesEnabled", "this=" << this);
	return (linphone_friend_subscribes_enabled(mFriend) == TRUE) ? true : false;
}

void FriendAPI::setIncSubscribePolicy(int policy) {
	CORE_MUTEX

	FBLOG_DEBUG("FriendAPI::setIncSubscribePolicy", "this=" << this << "\t" << "policy=" << policy);
	linphone_friend_set_inc_subscribe_policy(mFriend, (LinphoneSubscribePolicy)policy);
}

int FriendAPI::getIncSubscribePolicy() const {
	CORE_MUTEX

	FBLOG_DEBUG("FriendAPI::getIncSubscribePolicy", "this=" << this);
	return linphone_friend_get_inc_subscribe_policy(mFriend);
}

void FriendAPI::setRefKey(StringPtr const &key) {
	CORE_MUTEX

	FBLOG_DEBUG("FriendAPI::setRefKey", "this=" << this << "\t" << "key=" << key);
	linphone_friend_set_ref_key(mFriend, STRING_TO_CHARPTR(key));
}

StringPtr FriendAPI::getRefKey() const {
	CORE_MUTEX

	FBLOG_DEBUG("FriendAPI::getRefKey", "this=" << this);
	return CHARPTR_TO_STRING(linphone_friend_get_ref_key(mFriend));
}

void FriendAPI::edit() {
	CORE_MUTEX

	FBLOG_DEBUG("FriendAPI::edit", "this=" << this);
	linphone_friend_edit(mFriend);
}

void FriendAPI::done() {
	CORE_MUTEX

	FBLOG_DEBUG("FriendAPI::done", "this=" << this);
	linphone_friend_done(mFriend);
}

bool FriendAPI::inList() const {
	CORE_MUTEX

	FBLOG_DEBUG("FriendAPI::inList", "this=" << this);
	return (linphone_friend_in_list(mFriend) == TRUE) ? true : false;
}

PresenceModelAPIPtr FriendAPI::getPresenceModel() const {
	CORE_MUTEX

	FBLOG_DEBUG("FriendAPI::getPresenceModel", "this=" << this);
	const LinphonePresenceModel *model = linphone_friend_get_presence_model(mFriend);
	return getFactory()->getPresenceModel(model);
}

} // LinphoneWeb
