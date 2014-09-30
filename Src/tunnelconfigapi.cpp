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
 */

#include "tunnelconfigapi.h"
#include "coreapi.h"

#include "utils.h"
#include "factoryapi.h"

#include <linphone/linphone_tunnel.h>

namespace LinphoneWeb {

TunnelConfigAPI::TunnelConfigAPI(LinphoneTunnelConfig *tunnelConfig) :
		WrapperAPI(APIDescription(this)), mTunnelConfig(tunnelConfig) {
	FBLOG_DEBUG("TunnelConfigAPI::TunnelConfigAPI", "this=" << this << ", tunnelConfig=" << tunnelConfig);
}

TunnelConfigAPI::TunnelConfigAPI() :
		WrapperAPI(APIDescription(this)) {
	FBLOG_DEBUG("TunnelConfigAPI::TunnelConfigAPI", "this=" << this);
	mTunnelConfig = linphone_tunnel_config_new();
	if(mTunnelConfig == NULL) {
		throw std::invalid_argument("one/many parameters is/are invalid");
	}
}

void TunnelConfigAPI::initProxy() {
	registerProperty("delay", make_property(this, &TunnelConfigAPI::getDelay, &TunnelConfigAPI::setDelay));
	registerProperty("host", make_property(this, &TunnelConfigAPI::getHost, &TunnelConfigAPI::setHost));
	registerProperty("port", make_property(this, &TunnelConfigAPI::getPort, &TunnelConfigAPI::setPort));
	registerProperty("remoteUdpMirrorPort", make_property(this, &TunnelConfigAPI::getRemoteUdpMirrorPort, &TunnelConfigAPI::setRemoteUdpMirrorPort));
}

TunnelConfigAPI::~TunnelConfigAPI() {
	FBLOG_DEBUG("TunnelConfigAPI::~TunnelConfigAPI", "this=" << this);
	if (isOwned()) {
		if(mTunnelConfig != NULL) {
			linphone_tunnel_config_destroy(mTunnelConfig);
		}
	}
}

int TunnelConfigAPI::getDelay() const {
	CORE_MUTEX

	FBLOG_DEBUG("TunnelConfigAPI::getDelay", "this=" << this);
	return linphone_tunnel_config_get_delay(mTunnelConfig);
}

void TunnelConfigAPI::setDelay(int delay) {
	CORE_MUTEX

	FBLOG_DEBUG("TunnelConfigAPI::setDelay", "this=" << this << "\t" << "delay=" << delay);
	linphone_tunnel_config_set_delay(mTunnelConfig, delay);
}

StringPtr TunnelConfigAPI::getHost() const {
	CORE_MUTEX

	FBLOG_DEBUG("TunnelConfigAPI::getHost", "this=" << this);
	return CHARPTR_TO_STRING(linphone_tunnel_config_get_host(mTunnelConfig));
}

void TunnelConfigAPI::setHost(StringPtr const &host) {
	CORE_MUTEX

	FBLOG_DEBUG("TunnelConfigAPI::setHost", "this=" << this << "\t" << "host=" << host);
	linphone_tunnel_config_set_host(mTunnelConfig, STRING_TO_CHARPTR(host));
}

int TunnelConfigAPI::getPort() const {
	CORE_MUTEX

	FBLOG_DEBUG("TunnelConfigAPI::getPort", "this=" << this);
	return linphone_tunnel_config_get_port(mTunnelConfig);
}

void TunnelConfigAPI::setPort(int port) {
	CORE_MUTEX

	FBLOG_DEBUG("TunnelConfigAPI::setPort", "this=" << this << "\t" << "port=" << port);
	linphone_tunnel_config_set_port(mTunnelConfig, port);
}

int TunnelConfigAPI::getRemoteUdpMirrorPort() const {
	CORE_MUTEX

	FBLOG_DEBUG("TunnelConfigAPI::getRemoteUdpMirrorPort", "this=" << this);
	return linphone_tunnel_config_get_remote_udp_mirror_port(mTunnelConfig);
}

void TunnelConfigAPI::setRemoteUdpMirrorPort(int port) {
	CORE_MUTEX

	FBLOG_DEBUG("TunnelConfigAPI::setRemoteUdpMirrorPort", "this=" << this << "\t" << "port=" << port);
	linphone_tunnel_config_set_remote_udp_mirror_port(mTunnelConfig, port);
}

} // LinphoneWeb
