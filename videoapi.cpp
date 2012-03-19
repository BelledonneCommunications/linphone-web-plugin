#include <JSObject.h>
#include <variant_list.h>
#include <DOM/Document.h>
#include <global/config.h>
#include "videoapi.h"

///////////////////////////////////////////////////////////////////////////////
/// @fn videoAPI::videoAPI(const linphonePtr& plugin, const FB::BrowserHostPtr host)
///
/// @brief  Constructor for your JSAPI object.  You should register your methods, properties, and events
///         that should be accessible to Javascript from here.
///
/// @see FB::JSAPIAuto::registerMethod
/// @see FB::JSAPIAuto::registerProperty
/// @see FB::JSAPIAuto::registerEvent
///////////////////////////////////////////////////////////////////////////////
VideoAPI::VideoAPI(const videoPtr& plugin, const FB::BrowserHostPtr& host) :
		m_plugin(plugin), m_host(host) {
	FBLOG_DEBUG("videoAPI::videoAPI()", this);

	mWindow = VideoWindow::create();

	// Methods
	registerProperty("id", make_property(this, &VideoAPI::getId));
	registerMethod("setBackgroundColor", make_method(this, &VideoAPI::setBackgroundColor));
}

///////////////////////////////////////////////////////////////////////////////
/// @fn videoAPI::~videoAPI()
///
/// @brief  Destructor.  Remember that this object will not be released until
///         the browser is done with it; this will almost definitely be after
///         the plugin is released.
///////////////////////////////////////////////////////////////////////////////
VideoAPI::~VideoAPI() {
	FBLOG_DEBUG("videoAPI::~videoAPI()", this);
}

///////////////////////////////////////////////////////////////////////////////
/// @fn linphonePtr CoreAPI::getPlugin()
///
/// @brief  Gets a reference to the plugin that was passed in when the object
///         was created.  If the plugin has already been released then this
///         will throw a FB::script_error that will be translated into a
///         javascript exception in the page.
///////////////////////////////////////////////////////////////////////////////
videoPtr VideoAPI::getPlugin() {
	videoPtr plugin(m_plugin.lock());
	if (!plugin) {
		throw FB::script_error("The plugin is invalid");
	}
	return plugin;
}

void VideoAPI::setWindow(FB::PluginWindow *window) {
	mWindow->setWindow(window);
}

int VideoAPI::getId() {
	return mWindow->getId();
}

void VideoAPI::setBackgroundColor(int r, int g, int b) {
	mWindow->setBackgroundColor(r, g, b);
}
