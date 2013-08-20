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

#include <logging.h>
#include "videowindowmac.h"

namespace LinphoneWeb {

VideoWindowPtr VideoWindow::create() {
	return boost::make_shared<VideoWindowMac>();
}

VideoWindowMac::VideoWindowMac():
		mWindow(NULL), mBackgroundColor(nil) {
	FBLOG_DEBUG("VideoWindowMac::VideoWindowMac", "this=" << this);
}

VideoWindowMac::~VideoWindowMac() {
	FBLOG_DEBUG("VideoWindowMac::~VideoWindowMac", "this=" << this);
	if(mBackgroundColor != nil) {
		CGColorRelease(mBackgroundColor);
	}
}

void VideoWindowMac::setBackgroundColor(int r, int g, int b) {
	if(mBackgroundColor != nil) {
		CGColorRelease(mBackgroundColor);
		mBackgroundColor = nil;
	}
	mBackgroundColor = CGColorCreateGenericRGB(r/255.0f, g/255.0f, b/255.0f, 1.0f);
	drawBackground();
}

void VideoWindowMac::drawBackground() {
	[(CALayer *)mWindow->getDrawingPrimitive() setBackgroundColor:mBackgroundColor];
}
	
void VideoWindowMac::setWindow(FB::PluginWindow *window) {
	FBLOG_DEBUG("VideoWindowMac::setWindow", "this=" << this << "\t" << "window=" << window);
	FB::PluginWindowMac* wnd = reinterpret_cast<FB::PluginWindowMac*>(window);
	if (wnd) {
		mWindow = wnd;
		CALayer *layer = (CALayer *)mWindow->getDrawingPrimitive();
		
		drawBackground();
		
		// Auto invalidate each 33ms
		FBLOG_DEBUG("VideoWindowMac::setWindow", "this=" << this << "\t" << "Model=" << mWindow->getDrawingModel());
		if (FB::PluginWindowMac::DrawingModelInvalidatingCoreAnimation == mWindow->getDrawingModel()) {
			wnd->StartAutoInvalidate(1.0/30.0);
		}
		
		FBLOG_DEBUG("VideoWindowMac::setWindow", "this=" << this << "\t" << "LOAD DRAWINGPRIMITIVE=" << layer);
	} else {
		if(mWindow) {
			if (FB::PluginWindowMac::DrawingModelInvalidatingCoreAnimation == mWindow->getDrawingModel()) {
				mWindow->StopAutoInvalidate();
			}
			CALayer *layer = (CALayer *)mWindow->getDrawingPrimitive();
			FBLOG_DEBUG("VideoWindowMac::setWindow", "this=" << this << "\t" << "UNLOAD DRAWINGPRIMITIVE=" << layer);
			mWindow = NULL;
		}
	}
}

void* VideoWindowMac::getNativeHandle() const {
	FBLOG_DEBUG("VideoWindowMac::getNativeHandle", "this=" << this);
	if(mWindow) {
		return (void*)mWindow->getDrawingPrimitive();
	} else {
		return NULL;
	}
}

bool VideoWindowMac::draw() {
	return false;
}

} // LinphoneWeb
