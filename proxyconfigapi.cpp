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

#include "proxyconfigapi.h"
#include "coreapi.h"
#include "utils.h"
#include "factoryapi.h"

ProxyConfigAPI::ProxyConfigAPI(LinphoneProxyConfig *proxyConfig):
		WrapperAPI(APIDescription(this)), mProxyConfig(proxyConfig) {
	mUsed = true;
	mConst = false;
	FBLOG_DEBUG("ProxyConfigAPI::ProxyConfigAPI", "this=" << this << "\t" << "proxyConfig=" << proxyConfig);
	linphone_proxy_config_set_user_data(mProxyConfig, this);
	initProxy();
}

ProxyConfigAPI::ProxyConfigAPI():
		WrapperAPI(APIDescription(this)) {
	mUsed = false;
	mConst = false;
	FBLOG_DEBUG("ProxyConfigAPI::ProxyConfigAPI", "this=" << this);
	mProxyConfig = linphone_proxy_config_new();
	linphone_proxy_config_set_user_data(mProxyConfig, this);
	initProxy();
}

void ProxyConfigAPI::initProxy() {
	registerProperty("core", make_property(this, &ProxyConfigAPI::getCore));
	registerProperty("contactParameters", make_property(this, &ProxyConfigAPI::getContactParameters, &ProxyConfigAPI::setContactParameters));
	registerProperty("dialEscapePlus", make_property(this, &ProxyConfigAPI::getDialEscapePlus, &ProxyConfigAPI::setDialEscapePlus));
	registerProperty("dialPrefix", make_property(this, &ProxyConfigAPI::getDialPrefix, &ProxyConfigAPI::setDialPrefix));
	registerProperty("domain", make_property(this, &ProxyConfigAPI::getDomain));
	registerProperty("error", make_property(this, &ProxyConfigAPI::getError));
	registerProperty("expires", make_property(this, &ProxyConfigAPI::getExpires, &ProxyConfigAPI::setExpires));
	registerProperty("identity", make_property(this, &ProxyConfigAPI::getIdentity, &ProxyConfigAPI::setIdentity));
	registerProperty("publishEnabled", make_property(this, &ProxyConfigAPI::publishEnabled, &ProxyConfigAPI::enablePublish));
	registerProperty("registered", make_property(this, &ProxyConfigAPI::isRegistered));
	registerProperty("registerEnabled", make_property(this, &ProxyConfigAPI::registerEnabled, &ProxyConfigAPI::enableRegister));
	registerProperty("route", make_property(this, &ProxyConfigAPI::getRoute, &ProxyConfigAPI::setRoute));
	registerProperty("serverAddr", make_property(this, &ProxyConfigAPI::getServerAddr, &ProxyConfigAPI::setServerAddr));
	registerProperty("sipSetup", make_property(this, &ProxyConfigAPI::getSipSetup, &ProxyConfigAPI::setSipSetup));
	//sipSetupContext
	registerProperty("state", make_property(this, &ProxyConfigAPI::getState));

	registerMethod("edit", make_method(this, &ProxyConfigAPI::edit));
	registerMethod("done", make_method(this, &ProxyConfigAPI::done));

	registerMethod("normalizeNumber", make_method(this, &ProxyConfigAPI::normalizeNumber));
	registerMethod("refreshRegister", make_method(this, &ProxyConfigAPI::refreshRegister));
}

ProxyConfigAPI::~ProxyConfigAPI() {
	FBLOG_DEBUG("ProxyConfigAPI::~ProxyConfigAPI", "this=" << this);
	linphone_proxy_config_set_user_data(mProxyConfig, NULL);
	if (!mUsed) {
		linphone_proxy_config_destroy(mProxyConfig);
	}
}

bool ProxyConfigAPI::isRegistered() const {
	FBLOG_DEBUG("ProxyConfigAPI::isRegistered()", "this=" << this);
	return linphone_proxy_config_is_registered(mProxyConfig) == TRUE ? true : false;
}

CoreAPIPtr ProxyConfigAPI::getCore() const {
	FBLOG_DEBUG("ProxyConfigAPI::getCore()", "this=" << this);
	return mFactory->get(linphone_proxy_config_get_core(mProxyConfig));
}

std::string ProxyConfigAPI::getContactParameters() const {
	FBLOG_DEBUG("ProxyConfigAPI::getContactParameters()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_proxy_config_get_contact_parameters(mProxyConfig));
}

void ProxyConfigAPI::setContactParameters(const std::string &parameter) {
	FBLOG_DEBUG("ProxyConfigAPI::setContactParameters()", "this=" << this << "\t" << "parameter=" << parameter);
	linphone_proxy_config_set_contact_parameters(mProxyConfig, parameter.c_str());
}

bool ProxyConfigAPI::getDialEscapePlus() const {
	FBLOG_DEBUG("ProxyConfigAPI::getDialEscapePlus()", "this=" << this);
	return linphone_proxy_config_get_dial_escape_plus(mProxyConfig) == TRUE ? true : false;
}
void ProxyConfigAPI::setDialEscapePlus(bool escape) {
	FBLOG_DEBUG("ProxyConfigAPI::setDialEscapePlus()", "this=" << this << "\t" << "escape=" << escape);
	linphone_proxy_config_set_dial_escape_plus(mProxyConfig, escape ? TRUE : FALSE);
}

std::string ProxyConfigAPI::getDialPrefix() const {
	FBLOG_DEBUG("ProxyConfigAPI::getDialPrefix()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_proxy_config_get_dial_prefix(mProxyConfig));
}
void ProxyConfigAPI::setDialPrefix(const std::string &prefix) {
	FBLOG_DEBUG("ProxyConfigAPI::setDialEscapePlus()", "this=" << this << "\t" << "prefix=" << prefix);
	linphone_proxy_config_set_dial_prefix(mProxyConfig, prefix.c_str());
}

std::string ProxyConfigAPI::getDomain() const {
	FBLOG_DEBUG("ProxyConfigAPI::getDomain()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_proxy_config_get_domain(mProxyConfig));
}

bool ProxyConfigAPI::publishEnabled() const {
	FBLOG_DEBUG("ProxyConfigAPI::publishEnabled()", "this=" << this);
	return linphone_proxy_config_publish_enabled(mProxyConfig) == TRUE? true : false;
}
void ProxyConfigAPI::enablePublish(bool enable) {
	FBLOG_DEBUG("ProxyConfigAPI::enablePublish()", "this=" << this << "\t" << "enable=" << enable);
	linphone_proxy_config_enable_publish(mProxyConfig, enable ? TRUE : FALSE);
}

std::string ProxyConfigAPI::getSipSetup() const {
	FBLOG_DEBUG("ProxyConfigAPI::getSipSetup()", "this=" << this);
	SipSetup *setup = linphone_proxy_config_get_sip_setup(mProxyConfig);
	return CHARPTR_TO_STRING((setup != NULL) ? setup->name : NULL);
}
void ProxyConfigAPI::setSipSetup(const std::string &sip_setup) {
	FBLOG_DEBUG("ProxyConfigAPI::setDialEscapePlus()", "this=" << this << "\t" << "sip_setup=" << sip_setup);
	linphone_proxy_config_set_sip_setup(mProxyConfig, sip_setup.c_str());
}

int ProxyConfigAPI::getError() const {
	FBLOG_DEBUG("ProxyConfigAPI::getError()", "this=" << this);
	return linphone_proxy_config_get_error(mProxyConfig);
}

void ProxyConfigAPI::setServerAddr(const std::string &server_addr) {
	FBLOG_DEBUG("ProxyConfigAPI::setServerAddr()", "this=" << this << "\t" << "server_addr=" << server_addr);
	linphone_proxy_config_set_server_addr(mProxyConfig, server_addr.c_str());
}
std::string ProxyConfigAPI::getServerAddr() const {
	FBLOG_DEBUG("ProxyConfigAPI::getServerAddr()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_proxy_config_get_addr(mProxyConfig));
}

void ProxyConfigAPI::setIdentity(const std::string &identity) {
	FBLOG_DEBUG("ProxyConfigAPI::setIdentity()", "this=" << this << "\t" << "identity=" << identity);
	linphone_proxy_config_set_identity(mProxyConfig, identity.c_str());
}
std::string ProxyConfigAPI::getIdentity() const {
	FBLOG_DEBUG("ProxyConfigAPI::getIdentity()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_proxy_config_get_identity(mProxyConfig));
}

void ProxyConfigAPI::setRoute(const std::string &route) {
	FBLOG_DEBUG("ProxyConfigAPI::setRoute()", "this=" << this << "\t" << "route=" << route);
	linphone_proxy_config_set_route(mProxyConfig, route.c_str());
}
std::string ProxyConfigAPI::getRoute() const {
	FBLOG_DEBUG("ProxyConfigAPI::getRoute()", "this=" << this);
	return CHARPTR_TO_STRING(linphone_proxy_config_get_route(mProxyConfig));
}

void ProxyConfigAPI::setExpires(int expires) {
	FBLOG_DEBUG("ProxyConfigAPI::expires()", "this=" << this << "\t" << "expires=" << expires);
	return linphone_proxy_config_expires(mProxyConfig, expires);
}
int ProxyConfigAPI::getExpires() const {
	FBLOG_DEBUG("ProxyConfigAPI::getExpires()", "this=" << this);
	return linphone_proxy_config_get_expires(mProxyConfig);
}

void ProxyConfigAPI::enableRegister(bool val) {
	FBLOG_DEBUG("ProxyConfigAPI::enableRegister()", "this=" << this << "\t" << "val=" << val);
	return linphone_proxy_config_enable_register(mProxyConfig, val ? TRUE : FALSE);
}
bool ProxyConfigAPI::registerEnabled() const {
	FBLOG_DEBUG("ProxyConfigAPI::registerEnabled()", "this=" << this);
	return linphone_proxy_config_register_enabled(mProxyConfig) == TRUE ? true : false;
}

int ProxyConfigAPI::getState() const {
	FBLOG_DEBUG("ProxyConfigAPI::getState()", "this=" << this);
	return linphone_proxy_config_get_state(mProxyConfig);
}

void ProxyConfigAPI::edit() {
	FBLOG_DEBUG("ProxyConfigAPI::edit()", "this=" << this);
	linphone_proxy_config_edit(mProxyConfig);
}

int ProxyConfigAPI::done() {
	FBLOG_DEBUG("ProxyConfigAPI::done()", "this=" << this);
	return linphone_proxy_config_done(mProxyConfig);
}

std::string ProxyConfigAPI::normalizeNumber(const std::string &username) const {
	FBLOG_DEBUG("ProxyConfigAPI::normalizeNumber()", "this=" << this << "\t" << "username=" << username);
	char buffer[512] = {0};
	linphone_proxy_config_normalize_number(mProxyConfig, username.c_str(), buffer, sizeof(buffer));
	return buffer;
}

void ProxyConfigAPI::refreshRegister() {
	FBLOG_DEBUG("ProxyConfigAPI::refreshRegister()", "this=" << this);
	linphone_proxy_config_refresh_register(mProxyConfig);
}
