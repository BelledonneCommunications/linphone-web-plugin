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

	// Property
	const std::string &getMagic();
	void setMagic(const std::string &magic);
	unsigned long getWindow();

	void setBackgroundColor(int r, int g, int b);

private:
	std::string m_magic;
	videoWeakPtr m_plugin;
	FB::BrowserHostPtr m_host;
	boost::shared_ptr<VideoWindow> mWindow;
};

#endif // H_VIDEOAPI
