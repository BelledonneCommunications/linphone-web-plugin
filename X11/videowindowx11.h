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

#ifndef H_VIDEOWINDOWX11
#define H_VIDEOWINDOWX11

#include <PluginWindow.h>
#include <boost/shared_ptr.hpp>
#include <gdk/gdk.h>
#include "../Src/videowindow.h"

namespace LinphoneWeb {

class VideoWindowX11: public VideoWindow {
public:
	VideoWindowX11();
	~VideoWindowX11();

	void setWindow(FB::PluginWindow *window);
	void* getNativeHandle() const;
	void setBackgroundColor(int r, int g, int b);
	bool draw();

	static gint expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer *data);
	static gint configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer *data);

private:
	void drawBackground();

	GdkColor mBackgroundColor;
	GdkPixmap *mPixmap;
	FB::PluginWindowX11* mWindow;
	int mExposeEventId;
	int ConfigureEventId;
};
	
} // LinphoneWeb

#endif // H_VIDEOWINDOWX11
