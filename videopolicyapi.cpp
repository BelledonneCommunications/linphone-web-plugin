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

#include "videopolicyapi.h"

namespace LinphoneWeb {

VideoPolicyAPI::VideoPolicyAPI():
	WrapperAPI(APIDescription(this)) {
	FBLOG_DEBUG("VideoPolicyAPI::VideoPolicyAPI", "this=" << this);
	mVideoPolicy.automatically_initiate = false;
	mVideoPolicy.automatically_accept = false;
}

void VideoPolicyAPI::initProxy() {
	registerProperty("automaticallyInitiate", make_property(this, &VideoPolicyAPI::getAutomaticallyInitiate, &VideoPolicyAPI::setAutomaticallyInitiate));
	registerProperty("automaticallyAccept", make_property(this, &VideoPolicyAPI::getAutomaticallyAccept, &VideoPolicyAPI::setAutomaticallyAccept));
}

VideoPolicyAPI::~VideoPolicyAPI() {
	FBLOG_DEBUG("VideoPolicyAPI::~VideoPolicyAPI", "this=" << this);
}

void VideoPolicyAPI::setAutomaticallyInitiate(bool enable) {
	FBLOG_DEBUG("VideoPolicyAPI::setAutomaticallyInitiate", "this=" << this << "\t" << "enable=" << enable);
	mVideoPolicy.automatically_initiate = (enable)? TRUE : FALSE;
}

bool VideoPolicyAPI::getAutomaticallyInitiate() const {
	FBLOG_DEBUG("VideoPolicyAPI::getAutomaticallyInitiate", "this=" << this);
	return (mVideoPolicy.automatically_initiate == TRUE)? true : false;
}

void VideoPolicyAPI::setAutomaticallyAccept(bool enable) {
	FBLOG_DEBUG("VideoPolicyAPI::setAutomaticallyAccept", "this=" << this << "\t" << "enable=" << enable);
	mVideoPolicy.automatically_accept = (enable)? TRUE : FALSE;
}

bool VideoPolicyAPI::getAutomaticallyAccept() const {
	FBLOG_DEBUG("VideoPolicyAPI::getAutomaticallyAccept", "this=" << this);
	return (mVideoPolicy.automatically_accept == TRUE)? true : false;
}

} // LinphoneWeb
