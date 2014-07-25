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

#include <FactoryBase.h>
#include <boost/make_shared.hpp>
#include <global/config.h>
#include "coreplugin.h"
#include "videoplugin.h"
#include "whiteboard.h"

namespace LinphoneWeb {
	
#if defined(WIN32) && defined(DEBUG)
#include <Windows.h>
#include <Dbghelp.h>
#include <Strsafe.h>

#ifdef LW_DEBUG_GENERATE_DUMPS
void GenerateDump(EXCEPTION_POINTERS* pExceptionPointers) {
	BOOL bMiniDumpSuccessful;
	CHAR szPath[MAX_PATH]; 
	CHAR szFileName[MAX_PATH]; 
	CHAR* szAppName = FBSTRING_PluginFileName;
	CHAR* szVersion = FBSTRING_PLUGIN_GIT_REVISION;
	DWORD dwBufferSize = MAX_PATH;
	HANDLE hDumpFile;
	SYSTEMTIME stLocalTime;
	MINIDUMP_EXCEPTION_INFORMATION ExpParam;

	// Dynamic get function
	auto hDbgHelp = LoadLibraryA("dbghelp");
	if(hDbgHelp == NULL)
		return;
	auto pMiniDumpWriteDump = (decltype(&MiniDumpWriteDump))GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
	if(pMiniDumpWriteDump == NULL)
		return;

	GetLocalTime(&stLocalTime);

	GetTempPathA(dwBufferSize, szPath);
	//ExpandEnvironmentStringsA("%SYSTEMDRIVE%", szPath, MAX_PATH);
	//StringCchCatA(szPath, MAX_PATH, "\\TEMP\\");

	StringCchPrintfA(szFileName, MAX_PATH, "%s%s-%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp", 
			   szPath, szAppName, szVersion, 
			   stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay, 
			   stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond, 
			   GetCurrentProcessId(), GetCurrentThreadId());

	FBLOG_DEBUG("GenerateDump", "ExceptionCode=" << pExceptionPointers->ExceptionRecord->ExceptionCode << "\t" << "file=" << szFileName);

	hDumpFile = CreateFileA(szFileName, GENERIC_READ|GENERIC_WRITE, 
				FILE_SHARE_WRITE|FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	if(hDumpFile != NULL) {
		ExpParam.ThreadId = GetCurrentThreadId();
		ExpParam.ExceptionPointers = pExceptionPointers;
		ExpParam.ClientPointers = TRUE;

		bMiniDumpSuccessful = pMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), 
						hDumpFile, MiniDumpWithDataSegs, &ExpParam, NULL, NULL);
		CloseHandle(hDumpFile);
	}
}

LONG CALLBACK unhandled_handler(EXCEPTION_POINTERS* e) {
	GenerateDump(e);
	return EXCEPTION_EXECUTE_HANDLER;
}
#endif // LW_DEBUG_GENERATE_DUMPS
#endif

class PluginFactory: public FB::FactoryBase {
private:

#if defined(WIN32) && defined(DEBUG) && defined(LW_DEBUG_GENERATE_DUMPS)
	void *mExceptionHandler;
#endif
	
	WhiteBoardPtr mWhiteBoard;
	
public:
	PluginFactory() {
		// Create the common whiteboard
		mWhiteBoard = boost::make_shared<WhiteBoard>();
	}
	
	///////////////////////////////////////////////////////////////////////////////
	/// @fn FB::PluginCorePtr createPlugin(std::string const &mimetype)
	///
	/// @brief  Creates a plugin object matching the provided mimetype
	///         If mimetype is empty, returns the default plugin
	///////////////////////////////////////////////////////////////////////////////
	FB::PluginCorePtr createPlugin(std::string const &mimetype) {
		FBLOG_DEBUG("createPlugin", mimetype);
		if (mimetype == FBSTRING_MIMEType_Video)
			return boost::make_shared<VideoPlugin>(mWhiteBoard);
		return boost::make_shared<CorePlugin>(mWhiteBoard);
	}

	///////////////////////////////////////////////////////////////////////////////
	/// @see FB::FactoryBase::globalPluginInitialize
	///////////////////////////////////////////////////////////////////////////////
	void globalPluginInitialize() {
		FBLOG_DEBUG("PluginFactory::globalPluginInitialize", "Start");
#if defined(WIN32) && defined(DEBUG) && defined(LW_DEBUG_GENERATE_DUMPS)
		mExceptionHandler = AddVectoredExceptionHandler(TRUE, unhandled_handler);
		FBLOG_DEBUG("PluginFactory::globalPluginInitialize", "Add exception handler(" << unhandled_handler << ")=" << mExceptionHandler);
#endif
		srand((unsigned int)time(NULL));

		CorePlugin::StaticInitialize();
		VideoPlugin::StaticInitialize();
		FBLOG_DEBUG("PluginFactory::globalPluginInitialize", "End");
	}

	///////////////////////////////////////////////////////////////////////////////
	/// @see FB::FactoryBase::globalPluginDeinitialize
	///////////////////////////////////////////////////////////////////////////////
	void globalPluginDeinitialize() {
		FBLOG_DEBUG("PluginFactory::globalPluginDeinitialize", "Start");

		CorePlugin::StaticDeinitialize();
		VideoPlugin::StaticDeinitialize();

#if defined(WIN32) && defined(DEBUG) && defined(LW_DEBUG_GENERATE_DUMPS)
		RemoveVectoredExceptionHandler(mExceptionHandler);
		FBLOG_DEBUG("PluginFactory::globalPluginInitialize", "Remove exception handler=" << mExceptionHandler);
#endif

		FBLOG_DEBUG("PluginFactory::globalPluginDeinitialize", "End");
	}

	void getLoggingMethods(FB::Log::LogMethodList& outMethods) {
#ifdef DEBUG
#ifndef WIN32
		// The next line will enable logging to the console (think: printf).
		outMethods.push_back(std::make_pair(FB::Log::LogMethod_Console, std::string()));
#else //WIN32
		CHAR szPath[MAX_PATH]; 
		CHAR szFileName[MAX_PATH]; 
		CHAR* szAppName = FBSTRING_PluginFileName;
		CHAR* szVersion = FBSTRING_PLUGIN_GIT_REVISION;
		DWORD dwBufferSize = MAX_PATH;
		SYSTEMTIME stLocalTime;

		GetLocalTime(&stLocalTime);
		GetTempPathA(dwBufferSize, szPath);
		//ExpandEnvironmentStringsA("%SYSTEMDRIVE%", szPath, MAX_PATH);
		//StringCchCatA(szPath, MAX_PATH, "/TEMP/");

		StringCchPrintfA(szFileName, MAX_PATH, "%s%s-%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld_FB.log", 
			szPath, szAppName, szVersion, 
			stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
			stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond,
			GetCurrentProcessId(), GetCurrentThreadId());
		outMethods.push_back(std::make_pair(FB::Log::LogMethod_File, std::string(szFileName)));
#endif //WIN32
#endif //DEBUG
	}

	FB::Log::LogLevel getLogLevel() {
#ifdef DEBUG
		return FB::Log::LogLevel_Debug; // Now Debug and above is logged.
#else //DEBUG
		return FB::Log::LogLevel_Error;
#endif //DEBUG
	}
};

	
} // LinphoneWeb
	
///////////////////////////////////////////////////////////////////////////////
/// @fn getFactoryInstance()
///
/// @brief  Returns the factory instance for this plugin module
///////////////////////////////////////////////////////////////////////////////
FB::FactoryBasePtr getFactoryInstance() {
	static boost::shared_ptr<LinphoneWeb::PluginFactory> factory = boost::make_shared<LinphoneWeb::PluginFactory>();
	return factory;
}

