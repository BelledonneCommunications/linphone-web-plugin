/*!
 Linphone Web - Web plugin of Linphone an audio/video SIP phone
 Copyright (C) 2012  Yann Diorcet <yann.diorcet@linphone.org>

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
 */

#include <JSObject.h>
#include <variant_list.h>
#include <DOM/Document.h>
#include <global/config.h>
#include "videoapi.h"
#include "utils.h"

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
		JSAPIAuto(APIDescription(this)), m_plugin(plugin), m_host(host) {
	FBLOG_DEBUG("videoAPI::videoAPI()", this);

	mWindow = VideoWindow::create();

	// Methods
	registerProperty("magic", make_property(this, &VideoAPI::getMagic, &VideoAPI::setMagic));
	registerProperty("window", make_property(this, &VideoAPI::getWindow));
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
	FBLOG_DEBUG("videoAPI::setWindow()", "window=" << window);
	mWindow->setWindow(window);
}

const std::string &VideoAPI::getMagic() {
	FBLOG_DEBUG("VideoAPI::getMagic()", "");
	return m_magic;
}

void VideoAPI::setMagic(const std::string &magic) {
	FBLOG_DEBUG("VideoAPI::setMagic()", "magic=" << magic);
	m_magic = magic;
}

unsigned long VideoAPI::getWindow() {
	FBLOG_DEBUG("VideoAPI::getWindow()", "");
	return mWindow->getId();
}

void VideoAPI::setBackgroundColor(int r, int g, int b) {
	FBLOG_DEBUG("VideoAPI::setBackgroundColor()", "r=" << r << "\tg=" << g << "\tb=" << g);
	mWindow->setBackgroundColor(r, g, b);
}
