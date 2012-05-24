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

#include "videoplugin.h"
#include "videoapi.h"

///////////////////////////////////////////////////////////////////////////////
/// @fn video::StaticInitialize()
///
/// @brief  Called from PluginFactory::globalPluginInitialize()
///
/// @see FB::FactoryBase::globalPluginInitialize
///////////////////////////////////////////////////////////////////////////////
void video::StaticInitialize() {
	// Place one-time initialization stuff here; As of FireBreath 1.4 this should only
	// be called once per process
}

///////////////////////////////////////////////////////////////////////////////
/// @fn video::StaticInitialize()
///
/// @brief  Called from PluginFactory::globalPluginDeinitialize()
///
/// @see FB::FactoryBase::globalPluginDeinitialize
///////////////////////////////////////////////////////////////////////////////
void video::StaticDeinitialize() {
	// Place one-time deinitialization stuff here. As of FireBreath 1.4 this should
	// always be called just before the plugin library is unloaded
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  video constructor.  Note that your API is not available
///         at this point, nor the window.  For best results wait to use
///         the JSAPI object until the onPluginReady method is called
///////////////////////////////////////////////////////////////////////////////
video::video(): mWindow(NULL){
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  linphone destructor.
///////////////////////////////////////////////////////////////////////////////
video::~video() {
	// This is optional, but if you reset m_api (the shared_ptr to your JSAPI
	// root object) and tell the host to free the retained JSAPI objects then
	// unless you are holding another shared_ptr reference to your JSAPI object
	// they will be released here.
	releaseRootJSAPI();
	m_host->freeRetainedObjects();
}

void video::onPluginReady() {
	// When this is called, the BrowserHost is attached, the JSAPI object is
	// created, and we are ready to interact with the page and such.  The
	// PluginWindow may or may not have already fire the AttachedEvent at
	// this point.
}

void video::shutdown() {
	// This will be called when it is time for the plugin to shut down;
	// any threads or anything else that may hold a shared_ptr to this
	// object should be released here so that this object can be safely
	// destroyed. This is the last point that shared_from_this and weak_ptr
	// references to this object will be valid
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  Creates an instance of the JSAPI object that provides your main
///         Javascript interface.
///
/// Note that m_host is your BrowserHost and shared_ptr returns a
/// FB::PluginCorePtr, which can be used to provide a
/// boost::weak_ptr<linphone> for your JSAPI class.
///
/// Be very careful where you hold a shared_ptr to your plugin class from,
/// as it could prevent your plugin class from getting destroyed properly.
///////////////////////////////////////////////////////////////////////////////
FB::JSAPIPtr video::createJSAPI() {
	FBLOG_DEBUG("video::createJSAPI()", this);
	// m_host is the BrowserHost

	mVideo = boost::make_shared<VideoAPI>(FB::ptr_cast<video>(shared_from_this()), m_host);
	if (mWindow) {
		mVideo->setWindow(mWindow);
	}
	return mVideo;
}

bool video::setReady () {
	FB::VariantMap::iterator fnd = m_params.find("magic");
    if (fnd != m_params.end()) {
		if(fnd->second.is_of_type<std::string>()) {
			FB::ptr_cast<VideoAPI>(getRootJSAPI())->setMagic(fnd->second.convert_cast<std::string>());
		}
	}
	return PluginCore::setReady();
}

bool video::onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *win) {
	//printf("Mouse down at: %d, %d\n", evt->m_x, evt->m_y);
	return false;
}

bool video::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *win) {
	//printf("Mouse up at: %d, %d\n", evt->m_x, evt->m_y);
	return false;
}

bool video::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *win) {
	//printf("Mouse move at: %d, %d\n", evt->m_x, evt->m_y);
	return false;
}
bool video::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *win) {
	FBLOG_DEBUG("video::onWindowAttached()", this);
	mWindow = win;
	if (mVideo) {
		mVideo->setWindow(mWindow);
	}
	return true;
}

bool video::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *win) {
	FBLOG_DEBUG("video::onWindowDetached()", this);
	mWindow = NULL;
	if (mVideo) {
		mVideo->setWindow(mWindow);
	}
	return true;
}

bool video::onDraw(FB::RefreshEvent *evt, FB::PluginWindow* win) {
	return false;
}

