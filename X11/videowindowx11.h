#ifndef H_VIDEOWINDOWX11
#define H_VIDEOWINDOWX11

#include <PluginWindow.h>
#include <boost/shared_ptr.hpp>
#include <gdk/gdk.h>
#include "../videowindow.h"

class VideoWindowX11: public VideoWindow {
public:
	VideoWindowX11();
	~VideoWindowX11();

	void setWindow(FB::PluginWindow *window);
	unsigned long getId();
	void setBackgroundColor(int r, int g, int b);

	static gint expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer *data);
	static gint configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer *data);

private:
	GdkPixmap *mPixmap;
	GtkWidget *mGtkWidget;
	int mExposeEventId;
	int ConfigureEventId;
};

#endif // H_VIDEOWINDOWX11
