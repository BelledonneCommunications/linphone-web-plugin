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

#include <logging.h>
#include "videowindowwin.h"

namespace LinphoneWeb {

VideoWindowPtr VideoWindow::create() {
	return boost::make_shared<VideoWindowWin>();
}

VideoWindowWin::VideoWindowWin(): mWin(NULL), mBrush(NULL) {
	FBLOG_DEBUG("VideoWindowWin::VideoWindowWin()", "this=" << this);
}

VideoWindowWin::~VideoWindowWin() {
	FBLOG_DEBUG("VideoWindowWin::~VideoWindowWin()", "this=" << this);
	if(mBrush) {
		DeleteObject(mBrush);
	}
}

void VideoWindowWin::setBackgroundColor(int r, int g, int b) {
	if(mBrush) {
		DeleteObject(mBrush);
	}
	mBrush = CreateSolidBrush(RGB(r, g, b));
	SetClassLongPtr(mWin->getHWND(), GCLP_HBRBACKGROUND, (LONG)mBrush);
	RedrawWindow(mWin->getHWND(), NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
}

bool VideoWindowWin::draw() {
	return false;
}

void VideoWindowWin::setWindow(FB::PluginWindow *window) {
	FBLOG_DEBUG("VideoWindowWin::setWindow()", "this=" << this << "\t" << "window=" << window);
	FB::PluginWindowWin* win = reinterpret_cast<FB::PluginWindowWin*>(window);
	if (win) {
		mWin = win;
		FBLOG_DEBUG( "VideoWindowWin::setWindow()", "this=" << this << "\t" << "Load HWND=" << mWin->getHWND());
	} else {
		FBLOG_DEBUG("VideoWindowWin::setWindow()", "this=" << this << "\t" << "Unload HWND=" << mWin->getHWND());
		mWin = NULL;
	}
}

unsigned long VideoWindowWin::getId() {
	FBLOG_DEBUG("VideoWindowWin::getId()", "this=" << this);
	return (unsigned long) mWin->getHWND();
}
	
} // LinphoneWeb
