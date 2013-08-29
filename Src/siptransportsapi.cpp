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

#include "siptransportsapi.h"

namespace LinphoneWeb {

SipTransportsAPI::SipTransportsAPI():
	WrapperAPI(APIDescription(this)) {
	FBLOG_DEBUG("SipTransportsAPI::SipTransportsAPI", "this=" << this);
	mSipTransports.udp_port = -1;
	mSipTransports.tcp_port = -1;
	mSipTransports.dtls_port = -1;
	mSipTransports.tls_port = -1;
}

void SipTransportsAPI::initProxy() {
	registerProperty("udpPort", make_property(this, &SipTransportsAPI::getUdpPort, &SipTransportsAPI::setUdpPort));
	registerProperty("tcpPort", make_property(this, &SipTransportsAPI::getTcpPort, &SipTransportsAPI::setTcpPort));
	registerProperty("dtlsPort", make_property(this, &SipTransportsAPI::getDtlsPort, &SipTransportsAPI::setDtlsPort));
	registerProperty("tlsPort", make_property(this, &SipTransportsAPI::getTlsPort, &SipTransportsAPI::setTlsPort));
}

SipTransportsAPI::~SipTransportsAPI() {
	FBLOG_DEBUG("SipTransportsAPI::~SipTransportsAPI", "this=" << this);
}

void SipTransportsAPI::setUdpPort(int port) {
	FBLOG_DEBUG("SipTransportsAPI::setUdpPort", "this=" << this << "\t" << "port=" << port);
	mSipTransports.udp_port = port;
}

int SipTransportsAPI::getUdpPort() const {
	FBLOG_DEBUG("SipTransportsAPI::getUdpPort", "this=" << this);
	return mSipTransports.udp_port;
}

void SipTransportsAPI::setTcpPort(int port) {
	FBLOG_DEBUG("SipTransportsAPI::setTcpPort", "this=" << this << "\t" << "port=" << port);
	mSipTransports.tcp_port = port;
}

int SipTransportsAPI::getTcpPort() const {
	FBLOG_DEBUG("SipTransportsAPI::getUdpPort", "this=" << this);
	return mSipTransports.tcp_port;
}

void SipTransportsAPI::setDtlsPort(int port) {
	FBLOG_DEBUG("SipTransportsAPI::setDtls", "this=" << this << "\t" << "port=" << port);
	mSipTransports.dtls_port = port;
}

int SipTransportsAPI::getDtlsPort() const {
	FBLOG_DEBUG("SipTransportsAPI::getUdpPort", "this=" << this);
	return mSipTransports.dtls_port;
}

void SipTransportsAPI::setTlsPort(int port) {
	FBLOG_DEBUG("SipTransportsAPI::setTlsPort", "this=" << this << "\t" << "port=" << port);
	mSipTransports.tls_port = port;
}

int SipTransportsAPI::getTlsPort() const {
	FBLOG_DEBUG("SipTransportsAPI::getUdpPort", "this=" << this);
	return mSipTransports.tls_port;
}

} // LinphoneWeb