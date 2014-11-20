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

#include "tunnelapi.h"
#include "tunnelconfigapi.h"
#include "coreapi.h"

#include "utils.h"
#include "factoryapi.h"

#include <linphone/linphone_tunnel.h>

namespace LinphoneWeb {

TunnelAPI::TunnelAPI(LinphoneTunnel *tunnel) :
		WrapperAPI(APIDescription(this)), mTunnel(tunnel) {
	FBLOG_DEBUG("TunnelAPI::TunnelAPI", "this=" << this << ", tunnel=" << tunnel);
}

void TunnelAPI::initProxy() {
	registerProperty("connected", make_property(this, &TunnelAPI::connected));
	registerProperty("mode", make_property(this, &TunnelAPI::getMode, &TunnelAPI::setMode));
	registerProperty("servers", make_property(this, &TunnelAPI::getServers));
	registerProperty("sipEnabled", make_property(this, &TunnelAPI::sipEnabled, &TunnelAPI::enableSip));

	registerMethod("addServer", make_method(this, &TunnelAPI::addServer));
	registerMethod("cleanServers", make_method(this, &TunnelAPI::cleanServers));
	registerMethod("reconnect", make_method(this, &TunnelAPI::reconnect));
	registerMethod("removeServer", make_method(this, &TunnelAPI::removeServer));
}

TunnelAPI::~TunnelAPI() {
	FBLOG_DEBUG("TunnelAPI::~TunnelAPI", "this=" << this);
}

bool TunnelAPI::connected() const {
	CORE_MUTEX

	FBLOG_DEBUG("TunnelAPI::connected", "this=" << this);
	return linphone_tunnel_connected(mTunnel) == TRUE ? true : false;
}

int TunnelAPI::getMode() const {
	CORE_MUTEX

	FBLOG_DEBUG("TunnelAPI::getMode", "this=" << this);
	return linphone_tunnel_get_mode(mTunnel);
}

void TunnelAPI::setMode(int mode) {
	CORE_MUTEX

	FBLOG_DEBUG("TunnelAPI::setMode", "this=" << this << "\t" << "mode=" << mode);
	linphone_tunnel_set_mode(mTunnel, (LinphoneTunnelMode)mode);
}

std::vector<TunnelConfigAPIPtr> TunnelAPI::getServers() const {
	CORE_MUTEX

	FBLOG_DEBUG("TunnelAPI::getServers", "this=" << this);
	std::vector<TunnelConfigAPIPtr> list;
	for (const MSList *node = linphone_tunnel_get_servers(mTunnel); node != NULL; node = ms_list_next(node)) {
		list.push_back(getFactory()->getTunnelConfig(reinterpret_cast<LinphoneTunnelConfig*>(node->data)));
	}
	return list;
}

void TunnelAPI::enableSip(bool enable) {
	CORE_MUTEX

	FBLOG_DEBUG("TunnelAPI::enableSip", "this=" << this << "\t" << "enable=" << enable);
	linphone_tunnel_enable_sip(mTunnel, enable ? TRUE : FALSE);
}

bool TunnelAPI::sipEnabled() const {
	CORE_MUTEX

	FBLOG_DEBUG("TunnelAPI::sipEnabled", "this=" << this);
	return linphone_tunnel_sip_enabled(mTunnel) == TRUE ? true : false;
}

void TunnelAPI::addServer(TunnelConfigAPIPtr const &server) {
	CORE_MUTEX

	FBLOG_DEBUG("TunnelAPI::addServer", "this=" << this << "\t" << "server=" << server);
	linphone_tunnel_add_server(mTunnel, server->getRef());
	server->disOwn();
}

void TunnelAPI::cleanServers() {
	CORE_MUTEX

	FBLOG_DEBUG("TunnelAPI::cleanServers", "this=" << this);
	linphone_tunnel_clean_servers(mTunnel);
}

void TunnelAPI::reconnect() {
	CORE_MUTEX

	FBLOG_DEBUG("TunnelAPI::reconnect", "this=" << this);
	linphone_tunnel_reconnect(mTunnel);
}

void TunnelAPI::removeServer(TunnelConfigAPIPtr const &server) {
	CORE_MUTEX

	FBLOG_DEBUG("TunnelAPI::removeServer", "this=" << this << "\t" << "server=" << server);
	linphone_tunnel_remove_server(mTunnel, server->getRef());
}

} // LinphoneWeb
