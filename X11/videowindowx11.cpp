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

void VideoWindowX11::draw_brush(GtkWidget *widget, GdkPixmap *pixmap, gdouble x, gdouble y) {
	GdkRectangle update_rect;

	update_rect.x = x - 5;
	update_rect.y = y - 5;
	update_rect.width = 10;
	update_rect.height = 10;
	gdk_draw_rectangle(pixmap, widget->style->black_gc, TRUE, update_rect.x, update_rect.y, update_rect.width, update_rect.height);
	gtk_widget_draw(widget, &update_rect);
}

gint VideoWindowX11::expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer *data) {
	VideoWindowX11 *context = (VideoWindowX11*) data;
	gdk_draw_pixmap(widget->window, widget->style->fg_gc[GTK_WIDGET_STATE (widget)], context->mPixmap, event->area.x, event->area.y, event->area.x, event->area.y, event->area.width, event->area.height);
	return FALSE;
}

gint VideoWindowX11::configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer *data) {
	VideoWindowX11 *context = (VideoWindowX11*) data;
	if (context->mPixmap)
		gdk_pixmap_unref(context->mPixmap);

	context->mPixmap = gdk_pixmap_new(widget->window, widget->allocation.width, widget->allocation.height, -1);
	gdk_draw_rectangle(context->mPixmap, widget->style->white_gc, TRUE, 0, 0, widget->allocation.width, widget->allocation.height);
	draw_brush(widget, context->mPixmap, 5, 5);
	return TRUE;
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
