/*!
 Linphone Web - Web plugin of Linphone an audio/video SIP phone
 Copyright (C) 2012-2013 Belledonne Communications
 
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
 
 
 Authors:
 - Yann Diorcet <diorcet.yann@gmail.com>
 
 */

#include "videoplugin.h"
#include "videoapi.h"

namespace LinphoneWeb {

///////////////////////////////////////////////////////////////////////////////
/// @fn VideoPlugin::StaticInitialize()
///
/// @brief  Called from PluginFactory::globalPluginInitialize()
///
/// @see FB::FactoryBase::globalPluginInitialize
///////////////////////////////////////////////////////////////////////////////
void VideoPlugin::StaticInitialize() {
	// Place one-time initialization stuff here; As of FireBreath 1.4 this should only
	// be called once per process
}

///////////////////////////////////////////////////////////////////////////////
/// @fn VideoPlugin::StaticInitialize()
///
/// @brief  Called from PluginFactory::globalPluginDeinitialize()
///
/// @see FB::FactoryBase::globalPluginDeinitialize
///////////////////////////////////////////////////////////////////////////////
void VideoPlugin::StaticDeinitialize() {
	// Place one-time deinitialization stuff here. As of FireBreath 1.4 this should
	// always be called just before the plugin library is unloaded
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  video constructor.  Note that your API is not available
///         at this point, nor the window.  For best results wait to use
///         the JSAPI object until the onPluginReady method is called
///////////////////////////////////////////////////////////////////////////////
VideoPlugin::VideoPlugin(WhiteBoardPtr const &whiteboard): mWindow(NULL), mWhiteBoard(whiteboard) {
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  linphone destructor.
///////////////////////////////////////////////////////////////////////////////
VideoPlugin::~VideoPlugin() {
	// This is optional, but if you reset m_api (the shared_ptr to your JSAPI
	// root object) and tell the host to free the retained JSAPI objects then
	// unless you are holding another shared_ptr reference to your JSAPI object
	// they will be released here.
	releaseRootJSAPI();
	m_host->freeRetainedObjects();
}

void VideoPlugin::onPluginReady() {
	FB::VariantMap::iterator fnd = m_params.find("magic");
	if (fnd != m_params.end()) {
		if(fnd->second.is_of_type<std::string>()) {
			FB::ptr_cast<VideoAPI>(getRootJSAPI())->setMagic(fnd->second.convert_cast<std::string>());
		}
	}
}

void VideoPlugin::shutdown() {
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
FB::JSAPIPtr VideoPlugin::createJSAPI() {
	FBLOG_DEBUG("VideoPlugin::createJSAPI", "this=" << this);
	FactoryAPIPtr factory = boost::make_shared<FactoryAPI>(FB::ptr_cast<FB::PluginCore>(shared_from_this()), mWhiteBoard);
	mVideo = factory->getVideo();
	if (mWindow) {
		mVideo->setWindow(mWindow);
	}
	return mVideo;
}

bool VideoPlugin::onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *win) {
	return false;
}

bool VideoPlugin::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *win) {
	return false;
}

bool VideoPlugin::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *win) {
	return false;
}
bool VideoPlugin::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *win) {
	FBLOG_DEBUG("VideoPlugin::onWindowAttached", "this=" << this << "\t" << "win=" << win);
	mWindow = win;
	if (mVideo) {
		mVideo->setWindow(mWindow);
	}
	return true;
}

bool VideoPlugin::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *win) {
	FBLOG_DEBUG("VideoPlugin::onWindowDetached", "this=" << this << "\t" << "win=" << win);
	mWindow = NULL;
	if (mVideo) {
		mVideo->setWindow(mWindow);
	}
	return true;
}

bool VideoPlugin::onDraw(FB::RefreshEvent *evt, FB::PluginWindow* win) {
	if (mVideo) {
		return mVideo->draw();
	}
	return false;
}
	
} // LinphoneWeb

