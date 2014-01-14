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

#include "addressapi.h"

#include "utils.h"
#include "factoryapi.h"

#include <stdexcept>

namespace LinphoneWeb {

AddressAPI::AddressAPI(LinphoneAddress *address) :
		WrapperAPI(APIDescription(this)), mAddress(address) {
	FBLOG_DEBUG("AddressAPI::AddressAPI", "this=" << this << "\t" << "address=" << address);
}

AddressAPI::AddressAPI(const LinphoneAddress *address) :
		WrapperAPI(APIDescription(this)), mAddress(const_cast<LinphoneAddress *>(address)) {
	FBLOG_DEBUG("AddressAPI::AddressAPI", "this=" << this << "\t" << "address=" << address);
}

AddressAPI::AddressAPI(StringPtr const &uri) :
		WrapperAPI(APIDescription(this)) {
	FBLOG_DEBUG("AddressAPI::AddressAPI", "this=" << this);
	mAddress = linphone_address_new(STRING_TO_CHARPTR(uri));
	if(mAddress == NULL) {
		throw std::invalid_argument("uri is invalid");
	}
}

void AddressAPI::initProxy() {
	registerMethod("asString", make_method(this, &AddressAPI::asString));
	registerMethod("asStringUriOnly", make_method(this, &AddressAPI::asStringUriOnly));
	registerMethod("clean", make_method(this, &AddressAPI::clean));
	registerMethod("clone", make_method(this, &AddressAPI::clone));
	if (isConst()) {
		registerProperty("displayName", make_property(this, &AddressAPI::getDisplayName));
		registerProperty("domain", make_property(this, &AddressAPI::getDomain));
		registerProperty("port", make_property(this, &AddressAPI::getPort));
		registerProperty("transport", make_property(this, &AddressAPI::getTransport));
		registerProperty("username", make_property(this, &AddressAPI::getUsername));
	} else {
		registerProperty("displayName", make_property(this, &AddressAPI::getDisplayName, &AddressAPI::setDisplayName));
		registerProperty("domain", make_property(this, &AddressAPI::getDomain, &AddressAPI::setDomain));
		registerProperty("port", make_property(this, &AddressAPI::getPort, &AddressAPI::setPort));
		registerProperty("transport", make_property(this, &AddressAPI::getTransport, &AddressAPI::setTransport));
		registerProperty("username", make_property(this, &AddressAPI::getUsername, &AddressAPI::setUsername));
	}
	registerProperty("scheme", make_property(this, &AddressAPI::getScheme));
}

AddressAPI::~AddressAPI() {
	FBLOG_DEBUG("AddressAPI::~AddressAPI", "this=" << this);
	if(isOwned()) {
		if(mAddress != NULL) {
			linphone_address_destroy(mAddress);
		}
	}
}

StringPtr AddressAPI::asString() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("AddressAPI::asString", "this=" << this);
	return CHARPTR_TO_STRING(linphone_address_as_string(mAddress));
}

StringPtr AddressAPI::asStringUriOnly() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("AddressAPI::asStringUriOnly", "this=" << this);
	return CHARPTR_TO_STRING(linphone_address_as_string_uri_only(mAddress));
}

void AddressAPI::clean() {
	CORE_MUTEX
	
	FBLOG_DEBUG("AddressAPI::clean", "this=" << this);
	linphone_address_clean(mAddress);
}

AddressAPIPtr AddressAPI::clone() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("AddressAPI::clone", "this=" << this);
	AddressAPIPtr ret(new AddressAPI(linphone_address_clone(mAddress)));
	ret->own();
	return ret;
}

StringPtr AddressAPI::getDisplayName() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("AddressAPI::getDisplayName", "this=" << this);
	return CHARPTR_TO_STRING(linphone_address_get_display_name(mAddress));
}

void AddressAPI::setDisplayName(StringPtr const &displayname) {
	CORE_MUTEX
	
	FBLOG_DEBUG("AddressAPI::setDisplayName", "this=" << this << "\t" << "displayname=" << displayname);
	linphone_address_set_display_name(mAddress, STRING_TO_CHARPTR(displayname));
}

StringPtr AddressAPI::getDomain() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("AddressAPI::getDomain", "this=" << this);
	return CHARPTR_TO_STRING(linphone_address_get_domain(mAddress));
}

void AddressAPI::setDomain(StringPtr const &domain) {
	CORE_MUTEX
	
	FBLOG_DEBUG("AddressAPI::setDomain", "this=" << this << "\t" << "domain=" << domain);
	linphone_address_set_domain(mAddress, STRING_TO_CHARPTR(domain));
}

int AddressAPI::getPort() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("AddressAPI::getPort", "this=" << this);
	return linphone_address_get_port(mAddress);
}

void AddressAPI::setPort(int port) {
	CORE_MUTEX
	
	FBLOG_DEBUG("AddressAPI::setPort", "this=" << this << "\t" << "port=" << port);
	linphone_address_set_port(mAddress, port);
}

StringPtr AddressAPI::getScheme() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("AddressAPI::getScheme", "this=" << this);
	return CHARPTR_TO_STRING(linphone_address_get_scheme(mAddress));
}

int AddressAPI::getTransport() const {
	CORE_MUTEX

	FBLOG_DEBUG("AddressAPI::getTransport", "this=" << this);
	return linphone_address_get_transport(mAddress);
}

void AddressAPI::setTransport(int transport) {
	CORE_MUTEX

	FBLOG_DEBUG("AddressAPI::setTransport", "this=" << this << "\t" << "transport=" << transport);
	linphone_address_set_transport(mAddress, (LinphoneTransportType)transport);
}

StringPtr AddressAPI::getUsername() const {
	CORE_MUTEX
	
	FBLOG_DEBUG("AddressAPI::getUsername", "this=" << this);
	return CHARPTR_TO_STRING(linphone_address_get_username(mAddress));
}

void AddressAPI::setUsername(StringPtr const &username) {
	CORE_MUTEX
	
	FBLOG_DEBUG("AddressAPI::setUsername", "this=" << this << "\t" << "username=" << username);
	linphone_address_set_username(mAddress, STRING_TO_CHARPTR(username));
}

} // LinphoneWeb
