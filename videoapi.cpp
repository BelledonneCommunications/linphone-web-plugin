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

#include "videoapi.h"

#include "factoryapi.h"
#include "utils.h"

namespace LinphoneWeb {

///////////////////////////////////////////////////////////////////////////////
/// @fn videoAPI::videoAPI(videoPtr const &plugin, const FB::BrowserHostPtr host)
///
/// @brief  Constructor for your JSAPI object.  You should register your methods, properties, and events
///         that should be accessible to Javascript from here.
///
/// @see FB::JSAPIAuto::registerMethod
/// @see FB::JSAPIAuto::registerProperty
/// @see FB::JSAPIAuto::registerEvent
///////////////////////////////////////////////////////////////////////////////
VideoAPI::VideoAPI():
		WrapperAPI(APIDescription(this)) {
	FBLOG_DEBUG("videoAPI::videoAPI", "this=" << this);

	mWindow = VideoWindow::create();
}

///////////////////////////////////////////////////////////////////////////////
/// @fn videoAPI::~videoAPI()
///
/// @brief  Destructor.  Remember that this object will not be released until
///         the browser is done with it; this will almost definitely be after
///         the plugin is released.
///////////////////////////////////////////////////////////////////////////////
VideoAPI::~VideoAPI() {
	FBLOG_DEBUG("videoAPI::~videoAPI", "this=" << this);
	
	// Remove window handle from whiteboard
	void * handle = mWindow->getNativeHandle();
	if(handle != NULL) {
		getFactory()->getWhiteBoard()->removeValue(handle);
	}
}
	
void VideoAPI::initProxy() {
	// Methods
	registerProperty("magic", make_property(this, &VideoAPI::getMagic, &VideoAPI::setMagic));
	registerProperty("window", make_property(this, &VideoAPI::getWindow));
	registerMethod("setBackgroundColor", make_method(this, &VideoAPI::setBackgroundColor));
}
	
void VideoAPI::setWindow(FB::PluginWindow *window) {
	FBLOG_DEBUG("videoAPI::setWindow", "this=" << this << "\t" << "window=" << window);

	// Remove window handle from whiteboard
	void * oldHandle = mWindow->getNativeHandle();
	if(oldHandle != NULL) {
		getFactory()->getWhiteBoard()->removeValue(oldHandle);
	}
	
	mWindow->setWindow(window);
	
	// Add window handle to whiteboard
	void * newHandle = mWindow->getNativeHandle();
	if(newHandle != NULL) {
		getFactory()->getWhiteBoard()->addValue(newHandle);
	}
}

std::string const &VideoAPI::getMagic() {
	FBLOG_DEBUG("VideoAPI::getMagic", "this=" << this);
	return mMagic;
}

void VideoAPI::setMagic(std::string const &magic) {
	FBLOG_DEBUG("VideoAPI::setMagic", "this=" << this << "\t" << "magic=" << magic);
	mMagic = magic;
}

WhiteBoard::IdType VideoAPI::getWindow() {
	FBLOG_DEBUG("VideoAPI::getWindow", "this=" << this);
	
	// Get id of window handle in whiteboard
	void * handle = mWindow->getNativeHandle();
	if(handle != NULL) {
		return getFactory()->getWhiteBoard()->getId(handle);
	} else {
		return WhiteBoard::NoId;
	}
}

void VideoAPI::setBackgroundColor(int r, int g, int b) {
	FBLOG_DEBUG("VideoAPI::setBackgroundColor", "this=" << this << "\t" << "r=" << r << "\tg=" << g << "\tb=" << g);
	mWindow->setBackgroundColor(r, g, b);
}

bool VideoAPI::draw() {
	// FBLOG_DEBUG("VideoAPI::draw", "this=" << this); // TOO VERBOSE
	return mWindow->draw();
}


} // LinphoneWeb