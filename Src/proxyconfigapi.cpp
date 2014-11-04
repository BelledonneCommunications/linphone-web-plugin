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

#include "proxyconfigapi.h"
#include "coreapi.h"

#include "utils.h"
#include "factoryapi.h"

namespace LinphoneWeb {

ProxyConfigAPI::ProxyConfigAPI(LinphoneProxyConfig *proxyConfig):
		WrapperAPI(APIDescription(this)), mProxyConfig(proxyConfig) {
	FBLOG_DEBUG("ProxyConfigAPI::ProxyConfigAPI", "this=" << this << "\t" << "proxyConfig=" << proxyConfig);
	linphone_proxy_config_set_user_data(mProxyConfig, this);
}

ProxyConfigAPI::ProxyConfigAPI():
		WrapperAPI(APIDescription(this)) {
	FBLOG_DEBUG("ProxyConfigAPI::ProxyConfigAPI", "this=" << this);
	mProxyConfig = linphone_proxy_config_new();
	linphone_proxy_config_set_user_data(mProxyConfig, this);
}

void ProxyConfigAPI::initProxy() {
	registerProperty("avpfMode", make_property(this, &ProxyConfigAPI::getAvpfMode, &ProxyConfigAPI::setAvpfMode));
	registerProperty("avpfRrInterval", make_property(this, &ProxyConfigAPI::getAvpfRrInterval, &ProxyConfigAPI::setAvpfRrInterval));
	registerProperty("core", make_property(this, &ProxyConfigAPI::getCore));
	registerProperty("contactParameters", make_property(this, &ProxyConfigAPI::getContactParameters, &ProxyConfigAPI::setContactParameters));
	registerProperty("dialEscapePlus", make_property(this, &ProxyConfigAPI::getDialEscapePlus, &ProxyConfigAPI::setDialEscapePlus));
	registerProperty("dialPrefix", make_property(this, &ProxyConfigAPI::getDialPrefix, &ProxyConfigAPI::setDialPrefix));
	registerProperty("domain", make_property(this, &ProxyConfigAPI::getDomain));
	registerProperty("error", make_property(this, &ProxyConfigAPI::getError));
	registerProperty("errorInfo", make_property(this, &ProxyConfigAPI::getErrorInfo));
	registerProperty("expires", make_property(this, &ProxyConfigAPI::getExpires, &ProxyConfigAPI::setExpires));
	registerProperty("identity", make_property(this, &ProxyConfigAPI::getIdentity, &ProxyConfigAPI::setIdentity));
	registerProperty("publishEnabled", make_property(this, &ProxyConfigAPI::publishEnabled, &ProxyConfigAPI::enablePublish));
	registerProperty("publishExpires", make_property(this, &ProxyConfigAPI::getPublishExpires, &ProxyConfigAPI::setPublishExpires));
	registerProperty("registerEnabled", make_property(this, &ProxyConfigAPI::registerEnabled, &ProxyConfigAPI::enableRegister));
	registerProperty("route", make_property(this, &ProxyConfigAPI::getRoute, &ProxyConfigAPI::setRoute));
	registerProperty("serverAddr", make_property(this, &ProxyConfigAPI::getServerAddr, &ProxyConfigAPI::setServerAddr));
	registerProperty("sipSetup", make_property(this, &ProxyConfigAPI::getSipSetup, &ProxyConfigAPI::setSipSetup));
	registerProperty("state", make_property(this, &ProxyConfigAPI::getState));

	registerMethod("edit", make_method(this, &ProxyConfigAPI::edit));
	registerMethod("done", make_method(this, &ProxyConfigAPI::done));

	registerMethod("normalizeNumber", make_method(this, &ProxyConfigAPI::normalizeNumber));
	registerMethod("refreshRegister", make_method(this, &ProxyConfigAPI::refreshRegister));
}

ProxyConfigAPI::~ProxyConfigAPI() {
	FBLOG_DEBUG("ProxyConfigAPI::~ProxyConfigAPI", "this=" << this);
	linphone_proxy_config_set_user_data(mProxyConfig, NULL);
	if (isOwned()) {
		if(mProxyConfig) {
			linphone_proxy_config_destroy(mProxyConfig);
		}
	}
}

int ProxyConfigAPI::getAvpfMode() const {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::getAvpfMode", "this=" << this);
	return (int)linphone_proxy_config_get_avpf_mode(mProxyConfig);
}

void ProxyConfigAPI::setAvpfMode(int mode) {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::setAvpfMode", "this=" << this << "\t" << "mode=" << mode);
	linphone_proxy_config_set_avpf_mode(mProxyConfig, (LinphoneAVPFMode)mode);
}

int ProxyConfigAPI::getAvpfRrInterval() const {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::getAvpfRrInterval", "this=" << this);
	return linphone_proxy_config_get_avpf_rr_interval(mProxyConfig);
}

void ProxyConfigAPI::setAvpfRrInterval(int interval) {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::setAvpfRrInterval", "this=" << this << "\t" << "interval=" << interval);
	linphone_proxy_config_set_avpf_rr_interval(mProxyConfig, interval);
}

CoreAPIPtr ProxyConfigAPI::getCore() const {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::getCore", "this=" << this);
	return getFactory()->getCore(linphone_proxy_config_get_core(mProxyConfig));
}

StringPtr ProxyConfigAPI::getContactParameters() const {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::getContactParameters", "this=" << this);
	return CHARPTR_TO_STRING(linphone_proxy_config_get_contact_parameters(mProxyConfig));
}

void ProxyConfigAPI::setContactParameters(StringPtr const &parameter) {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::setContactParameters", "this=" << this << "\t" << "parameter=" << parameter);
	linphone_proxy_config_set_contact_parameters(mProxyConfig, STRING_TO_CHARPTR(parameter));
}

bool ProxyConfigAPI::getDialEscapePlus() const {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::getDialEscapePlus", "this=" << this);
	return linphone_proxy_config_get_dial_escape_plus(mProxyConfig) == TRUE ? true : false;
}

void ProxyConfigAPI::setDialEscapePlus(bool escape) {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::setDialEscapePlus", "this=" << this << "\t" << "escape=" << escape);
	linphone_proxy_config_set_dial_escape_plus(mProxyConfig, escape ? TRUE : FALSE);
}

StringPtr ProxyConfigAPI::getDialPrefix() const {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::getDialPrefix", "this=" << this);
	return CHARPTR_TO_STRING(linphone_proxy_config_get_dial_prefix(mProxyConfig));
}

void ProxyConfigAPI::setDialPrefix(StringPtr const &prefix) {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::setDialEscapePlus", "this=" << this << "\t" << "prefix=" << prefix);
	linphone_proxy_config_set_dial_prefix(mProxyConfig, STRING_TO_CHARPTR(prefix));
}

StringPtr ProxyConfigAPI::getDomain() const {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::getDomain", "this=" << this);
	return CHARPTR_TO_STRING(linphone_proxy_config_get_domain(mProxyConfig));
}

bool ProxyConfigAPI::publishEnabled() const {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::publishEnabled", "this=" << this);
	return linphone_proxy_config_publish_enabled(mProxyConfig) == TRUE? true : false;
}

void ProxyConfigAPI::enablePublish(bool enable) {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::enablePublish", "this=" << this << "\t" << "enable=" << enable);
	linphone_proxy_config_enable_publish(mProxyConfig, enable ? TRUE : FALSE);
}

StringPtr ProxyConfigAPI::getSipSetup() const {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::getSipSetup", "this=" << this);
	SipSetup *setup = linphone_proxy_config_get_sip_setup(mProxyConfig);
	return CHARPTR_TO_STRING((setup != NULL) ? setup->name : NULL);
}

void ProxyConfigAPI::setSipSetup(StringPtr const &sip_setup) {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::setDialEscapePlus", "this=" << this << "\t" << "sip_setup=" << sip_setup);
	linphone_proxy_config_set_sip_setup(mProxyConfig, STRING_TO_CHARPTR(sip_setup));
}

int ProxyConfigAPI::getError() const {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::getError", "this=" << this);
	return linphone_proxy_config_get_error(mProxyConfig);
}

ErrorInfoAPIPtr ProxyConfigAPI::getErrorInfo() const {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::getErrorInfo", "this=" << this);
	return getFactory()->getErrorInfo(linphone_proxy_config_get_error_info(mProxyConfig));
}

void ProxyConfigAPI::setServerAddr(StringPtr const &server_addr) {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::setServerAddr", "this=" << this << "\t" << "server_addr=" << server_addr);
	linphone_proxy_config_set_server_addr(mProxyConfig, STRING_TO_CHARPTR(server_addr));
}

StringPtr ProxyConfigAPI::getServerAddr() const {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::getServerAddr", "this=" << this);
	return CHARPTR_TO_STRING(linphone_proxy_config_get_addr(mProxyConfig));
}

void ProxyConfigAPI::setIdentity(StringPtr const &identity) {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::setIdentity", "this=" << this << "\t" << "identity=" << identity);
	linphone_proxy_config_set_identity(mProxyConfig, STRING_TO_CHARPTR(identity));
}

StringPtr ProxyConfigAPI::getIdentity() const {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::getIdentity", "this=" << this);
	return CHARPTR_TO_STRING(linphone_proxy_config_get_identity(mProxyConfig));
}

void ProxyConfigAPI::setRoute(StringPtr const &route) {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::setRoute", "this=" << this << "\t" << "route=" << route);
	linphone_proxy_config_set_route(mProxyConfig, STRING_TO_CHARPTR(route));
}

StringPtr ProxyConfigAPI::getRoute() const {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::getRoute", "this=" << this);
	return CHARPTR_TO_STRING(linphone_proxy_config_get_route(mProxyConfig));
}

void ProxyConfigAPI::setExpires(int expires) {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::setExpires", "this=" << this << "\t" << "expires=" << expires);
	return linphone_proxy_config_set_expires(mProxyConfig, expires);
}

int ProxyConfigAPI::getExpires() const {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::getExpires", "this=" << this);
	return linphone_proxy_config_get_expires(mProxyConfig);
}

void ProxyConfigAPI::setPublishExpires(int expires) {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::setPublishExpires", "this=" << this << "\t" << "expires=" << expires);
	return linphone_proxy_config_set_publish_expires(mProxyConfig, expires);
}

int ProxyConfigAPI::getPublishExpires() const {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::getPublishExpires", "this=" << this);
	return linphone_proxy_config_get_publish_expires(mProxyConfig);
}

void ProxyConfigAPI::enableRegister(bool val) {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::enableRegister", "this=" << this << "\t" << "val=" << val);
	return linphone_proxy_config_enable_register(mProxyConfig, val ? TRUE : FALSE);
}

bool ProxyConfigAPI::registerEnabled() const {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::registerEnabled", "this=" << this);
	return linphone_proxy_config_register_enabled(mProxyConfig) == TRUE ? true : false;
}

int ProxyConfigAPI::getState() const {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::getState", "this=" << this);
	return linphone_proxy_config_get_state(mProxyConfig);
}

void ProxyConfigAPI::edit() {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::edit", "this=" << this);
	linphone_proxy_config_edit(mProxyConfig);
}

int ProxyConfigAPI::done() {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::done", "this=" << this);
	return linphone_proxy_config_done(mProxyConfig);
}

StringPtr ProxyConfigAPI::normalizeNumber(StringPtr const &username) const {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::normalizeNumber", "this=" << this << "\t" << "username=" << username);
	char buffer[512] = {0};
	linphone_proxy_config_normalize_number(mProxyConfig, STRING_TO_CHARPTR(username), buffer, sizeof(buffer));
	return std::string(buffer);
}

void ProxyConfigAPI::refreshRegister() {
	CORE_MUTEX

	FBLOG_DEBUG("ProxyConfigAPI::refreshRegister", "this=" << this);
	linphone_proxy_config_refresh_register(mProxyConfig);
}
	
} // LinphoneWeb
