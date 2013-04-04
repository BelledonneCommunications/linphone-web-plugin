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

#include "coreapi.h"
#include "coreplugin.h"
#include <boost/filesystem.hpp>
#include <global/config.h>
#ifdef WIN32
#include <Windows.h>
#include <Strsafe.h>
#endif //WIN32

#ifdef DEBUG
FILE * core::s_log_file = NULL;
void core::log(OrtpLogLevel lev, const char *fmt, va_list args) {
	const char *lname="undef";
	char *msg;
	if (s_log_file==NULL) s_log_file = stderr;
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
	fprintf(s_log_file, FBSTRING_PluginFileName"-%s-%s\r\n", lname, msg);
	fflush(s_log_file);
	ortp_free(msg);
}

void core::enableLog() {
#ifdef WIN32
	WCHAR szPath[MAX_PATH]; 
    WCHAR szFileName[MAX_PATH]; 
    WCHAR* szAppName = TEXT(FBSTRING_PluginFileName);
    WCHAR* szVersion = TEXT(FBSTRING_PLUGIN_VERSION);
	DWORD dwBufferSize = MAX_PATH;
	SYSTEMTIME stLocalTime;

	GetLocalTime(&stLocalTime);
	//GetTempPath(dwBufferSize, szPath);
	ExpandEnvironmentStrings(L"%SYSTEMDRIVE%", szPath, MAX_PATH);
	StringCchCat (szPath, MAX_PATH, L"\\TEMP\\");

    StringCchPrintf(szFileName, MAX_PATH, L"%s\\%s-%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.log", 
               szPath, szAppName, szVersion, 
               stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay, 
               stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond, 
               GetCurrentProcessId(), GetCurrentThreadId());
	s_log_file = _wfopen(szFileName, L"w+");

	linphone_core_enable_logs_with_cb(core::log);
#else
	linphone_core_enable_logs(stdout);
#endif //WIN32
}
#endif //DEBUG

void core::disableLog() {
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
/// @fn core::StaticInitialize()
///
/// @brief  Called from PluginFactory::globalPluginInitialize()
///
/// @see FB::FactoryBase::globalPluginInitialize
///////////////////////////////////////////////////////////////////////////////
void core::StaticInitialize() {
	// Place one-time initialization stuff here; As of FireBreath 1.4 this should only
	// be called once per process
#ifndef DEBUG
	disableLog();
#else //DEBUG
	enableLog();
#endif //DEBUG
	*((char*)0x0)=0;
}

///////////////////////////////////////////////////////////////////////////////
/// @fn core::StaticInitialize()
///
/// @brief  Called from PluginFactory::globalPluginDeinitialize()
///
/// @see FB::FactoryBase::globalPluginDeinitialize
///////////////////////////////////////////////////////////////////////////////
void core::StaticDeinitialize() {
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
core::core() {
}

///////////////////////////////////////////////////////////////////////////////
/// @brief  core destructor.
///////////////////////////////////////////////////////////////////////////////
core::~core() {
	// This is optional, but if you reset m_api (the shared_ptr to your JSAPI
	// root object) and tell the host to free the retained JSAPI objects then
	// unless you are holding another shared_ptr reference to your JSAPI object
	// they will be released here.
	releaseRootJSAPI();
	m_host->freeRetainedObjects();
}

void core::onPluginReady() {
	// When this is called, the BrowserHost is attached, the JSAPI object is
	// created, and we are ready to interact with the page and such.  The
	// PluginWindow may or may not have already fire the AttachedEvent at
	// this point.
}

void core::shutdown() {
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
/// boost::weak_ptr<core> for your JSAPI class.
///
/// Be very careful where you hold a shared_ptr to your plugin class from,
/// as it could prevent your plugin class from getting destroyed properly.
///////////////////////////////////////////////////////////////////////////////
FB::JSAPIPtr core::createJSAPI() {
	FBLOG_DEBUG("core::createJSAPI()", this);
	// m_host is the BrowserHost
	return boost::make_shared<CoreAPI>(FB::ptr_cast<core>(shared_from_this()), m_host);
}

void core::setFSPath(const std::string &path) {
	boost::filesystem::path fpath(path);
	fpath = fpath.parent_path();
	fpath /= std::string("linphoneweb/");
	boost::filesystem::current_path(fpath);
	FBLOG_DEBUG("core::setFSPath", "Change current directory: " << fpath.string());
}

bool core::setReady() {
	FB::VariantMap::iterator fnd = m_params.find("magic");
	if (fnd != m_params.end()) {
		if (fnd->second.is_of_type<std::string>()) {
			FB::ptr_cast<CoreAPI>(getRootJSAPI())->setMagic(fnd->second.convert_cast<std::string>());
		}
	}
	return PluginCore::setReady();
}

bool core::onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *) {
	//printf("Mouse down at: %d, %d\n", evt->m_x, evt->m_y);
	return false;
}

bool core::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *) {
	//printf("Mouse up at: %d, %d\n", evt->m_x, evt->m_y);
	return false;
}

bool core::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *) {
	//printf("Mouse move at: %d, %d\n", evt->m_x, evt->m_y);
	return false;
}
bool core::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *) {
	// The window is attached; act appropriately
	return false;
}

bool core::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *) {
	// The window is about to be detached; act appropriately
	return false;
}

