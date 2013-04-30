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

#import <AppKit/AppKit.h>

#include <logging.h>
#include "videowindowmac.h"

VideoWindowPtr VideoWindow::create() {
	return boost::make_shared<VideoWindowMac>();
}

VideoWindowMac::VideoWindowMac():
		mWindow(NULL) {
	FBLOG_DEBUG("VideoWindowMac::VideoWindowMac", "this=" << this);
}

VideoWindowMac::~VideoWindowMac() {
	FBLOG_DEBUG("VideoWindowMac::~VideoWindowMac", "this=" << this);
}

void VideoWindowMac::setBackgroundColor(int r, int g, int b) {
	CGColorRef bgc = CGColorCreateGenericRGB(r/255.0f, g/255.0f, b/255.0f, 1.0f);
	[(CALayer *)mWindow setBackgroundColor:bgc];
	CGColorRelease(bgc);
}

void VideoWindowMac::setWindow(FB::PluginWindow *window) {
	FBLOG_DEBUG("VideoWindowMac::setWindow", "this=" << this << "\t" << "window=" << window);
	FB::PluginWindowMac* wnd = reinterpret_cast<FB::PluginWindowMac*>(window);
	if (wnd) {
		FBLOG_DEBUG("VideoWindowMac::setWindow", "this=" << this << "\t" << "Model=" << wnd->getDrawingModel());
		mWindow = wnd->getDrawingPrimitive();
		if (FB::PluginWindowMac::DrawingModelInvalidatingCoreAnimation == wnd->getDrawingModel())
			wnd->StartAutoInvalidate(1.0/30.0);
		
		FBLOG_DEBUG("VideoWindowMac::setWindow", "this=" << this << "\t" << "LOAD DRAWINGPRIMITIVE=" << mWindow);
	} else {
		mWindow = NULL;
		FBLOG_DEBUG("VideoWindowMac::setWindow", "this=" << this << "\t" << "UNLOAD DRAWINGPRIMITIVE=" << mWindow);
	}
}

unsigned long VideoWindowMac::getId() {
	FBLOG_DEBUG("VideoWindowMac::getId", "this=" << this);
	return (unsigned long) mWindow;
}

bool VideoWindowMac::draw() {
	return false;
}