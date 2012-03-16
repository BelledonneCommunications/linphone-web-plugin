#ifndef H_VIDEOAPI
#define H_VIDEOAPI

#include <string>
#include <JSAPIAuto.h>
#include <BrowserHost.h>
#include <variant_list.h>
#include "videoplugin.h"
#include "videowindow.h"

class VideoAPI: public FB::JSAPIAuto {
public:
	VideoAPI(const videoPtr& plugin, const FB::BrowserHostPtr& host);
	~VideoAPI();

	void setWindow(FB::PluginWindow *window);
	videoPtr getPlugin();

	// Methods
	int init();

private:
	videoWeakPtr m_plugin;
	FB::BrowserHostPtr m_host;
	boost::shared_ptr<VideoWindow> mWindow;
};

#endif // H_VIDEOAPI
