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

#ifndef H_VIDEOWINDOWMAC
#define H_VIDEOWINDOWMAC

#import <AppKit/AppKit.h>

#include <PluginWindowMac.h>
#include <boost/shared_ptr.hpp>
#include "../Src/videowindow.h"

namespace LinphoneWeb {

class VideoWindowMac: public VideoWindow {
public:
	VideoWindowMac();
	~VideoWindowMac();

	void setWindow(FB::PluginWindow *window);
	void* getNativeHandle() const;
	void setBackgroundColor(int r, int g, int b);
	bool draw();

private:
	void drawBackground();
	
	FB::PluginWindowMac *mWindow;
	CGColorRef mBackgroundColor;
};
	
} // LinphoneWeb

#endif // H_VIDEOWINDOWMAC
