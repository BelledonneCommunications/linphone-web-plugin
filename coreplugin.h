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

#ifndef H_COREPLUGIN
#define H_COREPLUGIN

#include <PluginWindow.h>
#include <PluginEvents/MouseEvents.h>
#include <PluginEvents/AttachedEvent.h>
#include <PluginCore.h>

#include "whiteboard.h"
#include "factoryapi.h"
#include <ortp/logging.h>

namespace LinphoneWeb {

FB_FORWARD_PTR(CorePlugin)
class CorePlugin: public FB::PluginCore {
private:
#ifdef DEBUG
#ifdef WIN32
	static FILE *s_log_file;
#endif //WIN32
#endif //DEBUG
	static CorePluginWeakPtr s_log_plugin;
	static void log(OrtpLogLevel lev, const char *fmt, va_list args);
	static void enableLog();
	static void disableLog();
	
	WhiteBoardPtr mWhiteBoard;
	
public:
	static void StaticInitialize();
	static void StaticDeinitialize();

public:
	CorePlugin(WhiteBoardPtr const &whiteboard);
	virtual ~CorePlugin();

public:
	void onPluginReady();
	void shutdown();
	virtual FB::JSAPIPtr createJSAPI();
	// If you want your plugin to always be windowless, set this to true
	// If you want your plugin to be optionally windowless based on the
	// value of the "windowless" param tag, remove this method or return
	// FB::PluginCore::isWindowless()
	virtual bool isWindowless() {
		return true;
	}

	BEGIN_PLUGIN_EVENT_MAP()
	EVENTTYPE_CASE(FB::MouseDownEvent, onMouseDown, FB::PluginWindow)
	EVENTTYPE_CASE(FB::MouseUpEvent, onMouseUp, FB::PluginWindow)
	EVENTTYPE_CASE(FB::MouseMoveEvent, onMouseMove, FB::PluginWindow)
	EVENTTYPE_CASE(FB::MouseMoveEvent, onMouseMove, FB::PluginWindow)
	EVENTTYPE_CASE(FB::AttachedEvent, onWindowAttached, FB::PluginWindow)
	EVENTTYPE_CASE(FB::DetachedEvent, onWindowDetached, FB::PluginWindow)
	END_PLUGIN_EVENT_MAP()

	/** BEGIN EVENTDEF -- DON'T CHANGE THIS LINE **/
	virtual bool onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *);
	virtual bool onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *);
	virtual bool onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *);
	virtual bool onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *);
	virtual bool onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *);
	/** END EVENTDEF -- DON'T CHANGE THIS LINE **/
};
	
} // LinphoneWeb

#endif //H_COREPLUGIN
