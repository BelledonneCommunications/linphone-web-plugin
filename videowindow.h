#ifndef H_VIDEOWINDOW
#define H_VIDEOWINDOW

#include <PluginWindow.h>
#include <boost/shared_ptr.hpp>

class VideoWindow {
public:
	static boost::shared_ptr<VideoWindow> create();
	virtual ~VideoWindow() {

	}
	virtual unsigned long getId() = 0;
	virtual void setBackgroundColor(int r, int g, int b) = 0;
	virtual void setWindow(FB::PluginWindow *window) = 0;
};

#endif // H_VIDEOWINDOW
