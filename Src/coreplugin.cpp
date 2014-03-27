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

#include "coreapi.h"
#include "coreplugin.h"
#include <boost/filesystem.hpp>
#include <global/config.h>
#ifdef WIN32
#include <Windows.h>
#include <Strsafe.h>
#endif //WIN32

namespace LinphoneWeb {

#ifdef DEBUG
#ifdef WIN32
FILE * CorePlugin::s_log_file = NULL;
#endif //WIN32
#endif //DEBUG

CorePluginWeakPtr CorePlugin::s_log_plugin;

void CorePlugin::log(OrtpLogLevel lev, const char *fmt, va_list args) {
	const char *lname="undef";
	char *msg;
	switch(lev){
		case ORTP_DEBUG:
			lname="debug";
			break;
		case ORTP_MESSAGE:
			lname="message";
			break;
		case ORTP_WARNING:
			lname="warning";
			break;
		case ORTP_ERROR:
			lname="error";
			break;
		case ORTP_FATAL:
			lname="fatal";
			break;
		default:
			ortp_fatal("Bad level !");
	}
	msg = ortp_strdup_vprintf(fmt, args);
#ifdef DEBUG
#ifdef WIN32
	fprintf(s_log_file, FBSTRING_PluginFileName"-%s-%s\r\n", lname, msg);
	fflush(s_log_file);
#else //WIN32
	fprintf(stdout, FBSTRING_PluginFileName"-%s-%s\r\n", lname, msg);
	fflush(stdout);
#endif //WIN32
#endif //DEBUG
	CorePluginPtr plugin = s_log_plugin.lock();
	if(plugin) {
		CoreAPIPtr core = FB::ptr_cast<CoreAPI>(plugin->getRootJSAPI());
		if(core) {
			core->log(lname, msg);
		}
	}
	ortp_free(msg);
}

void CorePlugin::enableLog() {
#ifdef DEBUG
#ifdef WIN32
	WCHAR szPath[MAX_PATH];
	WCHAR szFileName[MAX_PATH];
	WCHAR* szAppName = TEXT(FBSTRING_PluginFileName);
	DWORD dwBufferSize = MAX_PATH;
	SYSTEMTIME stLocalTime;

	GetLocalTime(&stLocalTime);
	GetTempPath(dwBufferSize, szPath);
	//ExpandEnvironmentStrings(L"%SYSTEMDRIVE%", szPath, MAX_PATH);
	//StringCchCat(szPath, MAX_PATH, L"\\TEMP\\");

	StringCchPrintf(szFileName, MAX_PATH, L"%s\\%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.log",
		szPath, szAppName,
		stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
		stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond,
		GetCurrentProcessId(), GetCurrentThreadId());
	s_log_file = _wfopen(szFileName, L"w+");
#endif //WIN32
#endif //DEBUG
	linphone_core_enable_logs_with_cb(CorePlugin::log);
}

void CorePlugin::disableLog() {
	linphone_core_disable_logs();
#ifdef DEBUG
#ifdef WIN32
	if(s_log_file != NULL) {
		fclose(s_log_file);
	}
#endif //WIN32
#endif //DEBUG
}

///////////////////////////////////////////////////////////////////////////////
/// @fn CorePlugin::StaticInitialize()
///
/// @brief  Called from PluginFactory::globalPluginInitialize()
///
/// @see FB::FactoryBase::globalPluginInitialize
///////////////////////////////////////////////////////////////////////////////
void CorePlugin::StaticInitialize() {
	// Place one-time initialization stuff here; As of FireBreath 1.4 this should only
	// be called once per process
	enableLog();
}

///////////////////////////////////////////////////////////////////////////////
/// @fn CorePlugin::StaticInitialize()
///
/// @brief  Called from PluginFactory::globalPluginDeinitialize()
///
/// @see FB::FactoryBase::globalPluginDeinitialize
///////////////////////////////////////////////////////////////////////////////
void CorePlugin::StaticDeinitialize() {
	// Place one-time deinitialization stuff here. As of FireBreath 1.4 this should
	// always be called just before the plugin library is unloaded
#ifdef DEBUG
	disableLog();
#endif
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  core constructor.  Note that your API is not available
///         at this point, nor the window.  For best results wait to use
///         the JSAPI object until the onPluginReady method is called
///////////////////////////////////////////////////////////////////////////////
CorePlugin::CorePlugin(WhiteBoardPtr const &whiteboard): mWhiteBoard(whiteboard) {
	FBLOG_DEBUG("CorePlugin::CorePlugin", "this=" << this);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  core destructor.
///////////////////////////////////////////////////////////////////////////////
CorePlugin::~CorePlugin() {
	// This is optional, but if you reset m_api (the shared_ptr to your JSAPI
	// root object) and tell the host to free the retained JSAPI objects then
	// unless you are holding another shared_ptr reference to your JSAPI object
	// they will be released here.
	FBLOG_DEBUG("CorePlugin::~CorePlugin", "this=" << this);
	
	// Clean
	releaseRootJSAPI();
	m_host->freeRetainedObjects();
}

void CorePlugin::onPluginReady() {
	// Log
	CorePluginPtr plugin = s_log_plugin.lock();
	if(!plugin) {
		FBLOG_DEBUG("CorePlugin::onPluginReady", "Set as log plugin");
		s_log_plugin = FB::ptr_cast<CorePlugin>(shared_from_this());
	}
	
	FB::VariantMap::iterator fnd = m_params.find("magic");
	if (fnd != m_params.end()) {
		if (fnd->second.is_of_type<std::string>()) {
			FB::ptr_cast<CoreAPI>(getRootJSAPI())->setMagic(fnd->second.convert_cast<std::string>());
		}
	}
}

void CorePlugin::shutdown() {
	// This will be called when it is time for the plugin to shut down;
	// any threads or anything else that may hold a shared_ptr to this
	// object should be released here so that this object can be safely
	// destroyed. This is the last point that shared_from_this and weak_ptr
	// references to this object will be valid
	FBLOG_DEBUG("CorePlugin::shutdown", "this=" << this);
	
	// Log
	CorePluginPtr plugin = s_log_plugin.lock();
	if(plugin == FB::ptr_cast<CorePlugin>(shared_from_this())) {
		s_log_plugin.reset();
	}
	
	// Sometimes the object is release before shutdown
	getRootJSAPI()->shutdown();
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  Creates an instance of the JSAPI object that provides your main
///         Javascript interface.
///
/// Note that m_host is your BrowserHost and shared_ptr returns a
/// FB::PluginCorePtr, which can be used to provide a
/// boost::weak_ptr<core> for your JSAPI class.
///
/// Be very careful where you hold a shared_ptr to your plugin class from,
/// as it could prevent your plugin class from getting destroyed properly.
///////////////////////////////////////////////////////////////////////////////
FB::JSAPIPtr CorePlugin::createJSAPI() {
	FBLOG_DEBUG("CorePlugin::createJSAPI", "this=" << this);
	FactoryAPIPtr factory = boost::make_shared<FactoryAPI>(FB::ptr_cast<FB::PluginCore>(shared_from_this()), mWhiteBoard);
	CoreAPIPtr core = factory->getCore((LinphoneCore *)NULL);
#ifdef DEBUG
	// In debug initialize at startup for show logs
	core->getFileManager();
#endif
	return core;
}

bool CorePlugin::onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *) {
	//printf("Mouse down at: %d, %d\n", evt->m_x, evt->m_y);
	return false;
}

bool CorePlugin::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *) {
	//printf("Mouse up at: %d, %d\n", evt->m_x, evt->m_y);
	return false;
}

bool CorePlugin::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *) {
	//printf("Mouse move at: %d, %d\n", evt->m_x, evt->m_y);
	return false;
}
bool CorePlugin::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *) {
	// The window is attached; act appropriately
	return false;
}

bool CorePlugin::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *) {
	// The window is about to be detached; act appropriately
	return false;
}

} // LinphoneWeb
