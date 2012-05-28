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

#include <PluginWindowWin.h>
#include <logging.h>
#include "videowindowwin.h"

VideoWindowPtr VideoWindow::create() {
	return boost::make_shared<VideoWindowWin>();
}

VideoWindowWin::VideoWindowWin() {
	FBLOG_DEBUG("VideoWindowWin::VideoWindowWin()", "this=" << this);
}

VideoWindowWin::~VideoWindowWin() {
	FBLOG_DEBUG("VideoWindowWin::~VideoWindowWin()", "this=" << this);
}

void VideoWindowWin::setBackgroundColor(int r, int g, int b) {
}

void VideoWindowWin::setWindow(FB::PluginWindow *window) {
	FBLOG_DEBUG("VideoWindowWin::setWindow()", "this=" << this << "\t" << "window=" << window);
	FB::PluginWindowWin* wnd = reinterpret_cast<FB::PluginWindowWin*>(window);
	if (wnd) {
		mHwnd = wnd->getHWND();
		FBLOG_DEBUG("VideoWindowWin::setWindow()", "this=" << this << "\t" << "LOAD HWND=" << mHwnd);
	} else {
		mHwnd = NULL;
		FBLOG_DEBUG("VideoWindowWin::setWindow()", "this=" << this << "\t" << "UNLOAD HWND=" << mHwnd);
	}
}

unsigned long VideoWindowWin::getId() {
	FBLOG_DEBUG("VideoWindowWin::getId()", "this=" << this);
	return (unsigned long) mHwnd;
}
