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

#include "msvideosizeapi.h"
#include "coreapi.h"

#include "utils.h"
#include "factoryapi.h"

namespace LinphoneWeb {

MSVideoSizeAPI::MSVideoSizeAPI(MSVideoSize vs) :
		WrapperAPI(APIDescription(this)), mVideoSize(vs) {
	FBLOG_DEBUG("MSVideoSizeAPI::MSVideoSizeAPI", "this=" << this << "\t" << "vs.width=" << vs.width << "\t" << "vs.height=" << vs.height);
}

void MSVideoSizeAPI::initProxy() {
	registerProperty("height", make_property(this, &MSVideoSizeAPI::getHeight, &MSVideoSizeAPI::setHeight));
	registerProperty("width", make_property(this, &MSVideoSizeAPI::getWidth, &MSVideoSizeAPI::setWidth));
}

MSVideoSizeAPI::~MSVideoSizeAPI() {
	FBLOG_DEBUG("MSVideoSizeAPI::~MSVideoSizeAPI", "this=" << this);
}

int MSVideoSizeAPI::getHeight() const {
	FBLOG_DEBUG("MSVideoSizeAPI::getHeight", "this=" << this);
	return mVideoSize.height;
}

void MSVideoSizeAPI::setHeight(int height) {
	FBLOG_DEBUG("MSVideoSizeAPI::setHeight", "this=" << this << "\t" << "height=" << height);
	mVideoSize.height = height;
}

int MSVideoSizeAPI::getWidth() const {
	FBLOG_DEBUG("MSVideoSizeAPI::getWidth", "this=" << this);
	return mVideoSize.width;
}

void MSVideoSizeAPI::setWidth(int width) {
	FBLOG_DEBUG("MSVideoSizeAPI::setWidth", "this=" << this << "\t" << "width=" << width);
	mVideoSize.width = width;
}

} // LinphoneWeb
