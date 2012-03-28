#ifndef H_VIDEOWINDOWWIN
#define H_VIDEOWINDOWWIN

#include <PluginWindow.h>
#include <boost/shared_ptr.hpp>
#include "../videowindow.h"
#include <WinDef.h>

class VideoWindowWin: public VideoWindow {
public:
	VideoWindowWin();
	~VideoWindowWin();

	void setWindow(FB::PluginWindow *window);
	unsigned long getId();
	void setBackgroundColor(int r, int g, int b);

private:
	HWND mHwnd;
};

#endif // H_VIDEOWINDOWWIN
