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

#include <PluginWindowX11.h>
#include <logging.h>
#include "videowindowx11.h"
#include <gdk/gdkx.h>

namespace LinphoneWeb {

VideoWindowPtr VideoWindow::create() {
	return boost::make_shared<VideoWindowX11>();
}

VideoWindowX11::VideoWindowX11() :
		mPixmap(NULL), mWindow(NULL) {
	FBLOG_DEBUG("VideoWindowX11::VideoWindowX11()", "this=" << this);
}

VideoWindowX11::~VideoWindowX11() {
	FBLOG_DEBUG("VideoWindowX11::~VideoWindowX11()", "this=" << this);
	setWindow(NULL);
}

gint VideoWindowX11::expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer *data) {
	return FALSE;
}

gint VideoWindowX11::configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer *data) {
	VideoWindowX11 *context = (VideoWindowX11*) data;
	// Recreate pixmap
	if (context->mPixmap) {
		gdk_pixmap_unref(context->mPixmap);
		context->mPixmap = NULL;
	}
	context->mPixmap = gdk_pixmap_new(widget->window, widget->allocation.width, widget->allocation.height, -1);
	context->drawBackground();
	return TRUE;
}

void VideoWindowX11::setBackgroundColor(int r, int g, int b) {
	mBackgroundColor.red = r;
	mBackgroundColor.green = g;
	mBackgroundColor.blue = b;
	mBackgroundColor.pixel = 0;
	if (mWindow != NULL) {
		gtk_widget_modify_bg(mWindow->getWidget(), GTK_STATE_NORMAL, &mBackgroundColor);
	}
	drawBackground();
}

void VideoWindowX11::drawBackground() {
	GtkWidget *gtkWidget = mWindow->getWidget();
	if (mPixmap == NULL) {
		mPixmap = gdk_pixmap_new(gtkWidget->window, gtkWidget->allocation.width, gtkWidget->allocation.height, -1);
	}
	gdk_draw_rectangle(mPixmap, gtkWidget->style->bg_gc[GTK_STATE_NORMAL], TRUE, 0, 0, gtkWidget->allocation.width, gtkWidget->allocation.height);
}

void VideoWindowX11::setWindow(FB::PluginWindow *window) {
	FBLOG_DEBUG("VideoWindowX11::setWindow()", "this=" << this << "\t" << "window=" << window);
	FB::PluginWindowX11* wnd = reinterpret_cast<FB::PluginWindowX11*>(window);
	if (wnd) {
		mWindow = wnd;
		GtkWidget *gtkWidget = mWindow->getWidget();
		gtk_widget_modify_bg(gtkWidget, GTK_STATE_NORMAL, &mBackgroundColor);
		drawBackground();

		mExposeEventId = gtk_signal_connect(GTK_OBJECT(gtkWidget), "expose_event", (GtkSignalFunc) expose_event, this);
		ConfigureEventId = gtk_signal_connect(GTK_OBJECT(gtkWidget), "configure_event", (GtkSignalFunc) configure_event, this);
	} else {
		if (mWindow != NULL) {
			GtkWidget *gtkWidget = mWindow->getWidget();
			gtk_signal_disconnect(GTK_OBJECT(gtkWidget), mExposeEventId);
			gtk_signal_disconnect(GTK_OBJECT(gtkWidget), ConfigureEventId);
			if(mPixmap) {
				gdk_pixmap_unref(mPixmap);
				mPixmap = NULL;
			}
			mWindow = NULL;
		}
	}
}

void* VideoWindowX11::getNativeHandle() const {
	FBLOG_DEBUG("VideoWindowX11::getNativeHandle()", "this=" << this);
	if(mWindow) {
		return (void *)GDK_DRAWABLE_XID(gtk_widget_get_window(mWindow->getWidget()));
	} else {
		return NULL;
	}
}

bool VideoWindowX11::draw() {
	return false;
}

} // LinphoneWeb
