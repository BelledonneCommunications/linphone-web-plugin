#include <PluginWindowX11.h>
#include <logging.h>
#include "videowindowx11.h"
#include <gdk/gdkx.h>

boost::shared_ptr<VideoWindow> VideoWindow::create() {
	return boost::make_shared<VideoWindowX11>();
}

VideoWindowX11::VideoWindowX11() :
		mPixmap(NULL), mGtkWidget(NULL) {
	FBLOG_DEBUG("VideoWindow::VideoWindow()", this);
}

VideoWindowX11::~VideoWindowX11() {
	FBLOG_DEBUG("VideoWindow::~VideoWindow()", this);
}

gint VideoWindowX11::expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer *data) {
	//VideoWindowX11 *context = (VideoWindowX11*) data;
	//gdk_draw_pixmap(widget->window, widget->style->fg_gc[GTK_WIDGET_STATE (widget)], context->mPixmap, event->area.x, event->area.y, event->area.x, event->area.y, event->area.width, event->area.height);
	return FALSE;
}

gint VideoWindowX11::configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer *data) {
	VideoWindowX11 *context = (VideoWindowX11*) data;
	if (context->mPixmap)
		gdk_pixmap_unref(context->mPixmap);

	context->mPixmap = gdk_pixmap_new(widget->window, widget->allocation.width, widget->allocation.height, -1);
	gdk_draw_rectangle(context->mPixmap, widget->style->white_gc, TRUE, 0, 0, widget->allocation.width, widget->allocation.height);
	return TRUE;
}

void VideoWindowX11::setBackgroundColor(int r, int g, int b) {
	GdkColor  backgroundColor;
	backgroundColor.red = r;
	backgroundColor.green = g;
	backgroundColor.blue = b;
	backgroundColor.pixel = 0;
	gtk_widget_modify_bg (mGtkWidget, GTK_STATE_NORMAL, &backgroundColor);
	gdk_draw_rectangle(mPixmap, mGtkWidget->style->bg_gc[GTK_STATE_NORMAL], TRUE, 0, 0, mGtkWidget->allocation.width, mGtkWidget->allocation.height);
}

void VideoWindowX11::setWindow(FB::PluginWindow *window) {
	FBLOG_DEBUG("VideoWindow::setWindow()", this);
	FB::PluginWindowX11* wnd = reinterpret_cast<FB::PluginWindowX11*>(window);
	if (wnd) {
		mGtkWidget = wnd->getWidget();
		mExposeEventId = gtk_signal_connect(GTK_OBJECT(mGtkWidget), "expose_event", (GtkSignalFunc) expose_event, this);
		ConfigureEventId = gtk_signal_connect(GTK_OBJECT(mGtkWidget), "configure_event", (GtkSignalFunc) configure_event, this);
	} else {
		if (mGtkWidget != NULL) {
			gtk_signal_disconnect(GTK_OBJECT(mGtkWidget), mExposeEventId);
			gtk_signal_disconnect(GTK_OBJECT(mGtkWidget), ConfigureEventId);
			gdk_pixmap_unref(mPixmap);
			mGtkWidget = NULL;
		}
	}
}

unsigned long VideoWindowX11::getId() {
	FBLOG_DEBUG("VideoWindow::getId()", this);
	return (unsigned long) GDK_DRAWABLE_XID(gtk_widget_get_window(mGtkWidget));
}
