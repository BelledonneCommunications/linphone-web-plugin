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

#include "addressapi.h"
#include "utils.h"

AddressAPI::AddressAPI(LinphoneAddress *address) :
		JSAPIAuto(APIDescription(this)), mAddress(address) {
    mUsed = true;
    mConst = false;
	FBLOG_DEBUG("AddressAPI::AddressAPI", "this=" << this << "\t" << "address=" << address);
	initProxy();
}

AddressAPI::AddressAPI(const LinphoneAddress *address) :
		JSAPIAuto(APIDescription(this)), mAddress(const_cast<LinphoneAddress *>(address)) {
	FBLOG_DEBUG("AddressAPI::AddressAPI", "this=" << this << "\t" << "address=" << address);
    mUsed = true;
    mConst = true;
	initProxy();
}

AddressAPI::AddressAPI(const std::string &uri) :
		JSAPIAuto(APIDescription(this)) {
	FBLOG_DEBUG("AddressAPI::AddressAPI", "this=" << this);
	mAddress = linphone_address_new(uri.c_str());
    mUsed = false;
    mConst = false;
	initProxy();
}

void AddressAPI::initProxy() {
	registerMethod("asString", make_method(this, &AddressAPI::asString));
	registerMethod("asStringUriOnly", make_method(this, &AddressAPI::asStringUriOnly));
	registerMethod("clean", make_method(this, &AddressAPI::clean));
	registerMethod("clone", make_method(this, &AddressAPI::clone));
	if (mConst) {
		registerProperty("displayName", make_property(this, &AddressAPI::getDisplayName));
		registerProperty("domain", make_property(this, &AddressAPI::getDomain));
		registerProperty("port", make_property(this, &AddressAPI::getPort));
		registerProperty("portInt", make_property(this, &AddressAPI::getPortInt));
		registerProperty("username", make_property(this, &AddressAPI::getUsername));
	} else {
		registerProperty("displayName", make_property(this, &AddressAPI::getDisplayName, &AddressAPI::setDisplayName));
		registerProperty("domain", make_property(this, &AddressAPI::getDomain, &AddressAPI::setDomain));
		registerProperty("port", make_property(this, &AddressAPI::getPort, &AddressAPI::setPort));
		registerProperty("portInt", make_property(this, &AddressAPI::getPortInt, &AddressAPI::setPortInt));
		registerProperty("username", make_property(this, &AddressAPI::getUsername, &AddressAPI::setUsername));
	}
	registerProperty("scheme", make_property(this, &AddressAPI::getScheme));
}

AddressAPI::~AddressAPI() {
	FBLOG_DEBUG("AddressAPI::~AddressAPI", "this=" << this);
	if(!mUsed) {
		linphone_address_destroy(mAddress);
	}
}

std::string AddressAPI::asString() const {
	FBLOG_DEBUG("AddressAPI::asString()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_address_as_string(mAddress));
}

std::string AddressAPI::asStringUriOnly() const {
	FBLOG_DEBUG("AddressAPI::asStringUriOnly()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_address_as_string_uri_only(mAddress));
}

void AddressAPI::clean() {
	FBLOG_DEBUG("AddressAPI::clean()", "this=" << this);
	linphone_address_clean(mAddress);
}

AddressAPIPtr AddressAPI::clone() const {
	FBLOG_DEBUG("AddressAPI::clone()", "this=" << this);
	AddressAPIPtr ret(new AddressAPI(linphone_address_clone(mAddress)));
	ret->mUsed = false;
	return ret;
}

std::string AddressAPI::getDisplayName() const {
	FBLOG_DEBUG("AddressAPI::getDisplayName()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_address_get_display_name(mAddress));
}

void AddressAPI::setDisplayName(const std::string &displayname) {
	FBLOG_DEBUG("AddressAPI::setDisplayName()", "this=" << this << "\t" << "displayname=" << displayname);
	linphone_address_set_display_name(mAddress, displayname.c_str());
}

std::string AddressAPI::getDomain() const {
	FBLOG_DEBUG("AddressAPI::getDomain()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_address_get_domain(mAddress));
}

void AddressAPI::setDomain(const std::string &domain) {
	FBLOG_DEBUG("AddressAPI::setDomain()", "this=" << this << "\t" << "domain=" << domain);
	linphone_address_set_domain(mAddress, domain.c_str());
}

std::string AddressAPI::getPort() const {
	FBLOG_DEBUG("AddressAPI::getPort()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_address_get_port(mAddress));
}

void AddressAPI::setPort(const std::string &port) {
	FBLOG_DEBUG("AddressAPI::setPort()", "this=" << this << "\t" << "port=" << port);
	linphone_address_set_port(mAddress, port.c_str());
}

int AddressAPI::getPortInt() const {
	FBLOG_DEBUG("AddressAPI::getPortInt()", "this=" << this);
	return linphone_address_get_port_int(mAddress);
}

void AddressAPI::setPortInt(int port) {
	FBLOG_DEBUG("AddressAPI::setPortInt()", "this=" << this << "\t" << "port=" << port);
	linphone_address_set_port_int(mAddress, port);
}

std::string AddressAPI::getScheme() const {
	FBLOG_DEBUG("AddressAPI::getScheme()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_address_get_scheme(mAddress));
}

std::string AddressAPI::getUsername() const {
	FBLOG_DEBUG("AddressAPI::getUsername()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_address_get_username(mAddress));
}

void AddressAPI::setUsername(const std::string &username) {
	FBLOG_DEBUG("AddressAPI::setUsername()", "this=" << this << "\t" << "username=" << username);
	linphone_address_set_username(mAddress, username.c_str());
}
