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

#include <FactoryBase.h>
#include <boost/make_shared.hpp>
#include <global/config.h>
#include "coreplugin.h"
#include "videoplugin.h"

#ifdef DEBUG
#ifdef WIN32
#include <Windows.h>
#include <Dbghelp.h>
#include <Strsafe.h>
int GenerateDump(EXCEPTION_POINTERS* pExceptionPointers)
{
    BOOL bMiniDumpSuccessful;
	WCHAR szPath[MAX_PATH]; 
    WCHAR szFileName[MAX_PATH]; 
    WCHAR* szAppName = TEXT(FBSTRING_PluginFileName);
    WCHAR* szVersion = TEXT(FBSTRING_PLUGIN_VERSION);
	DWORD dwBufferSize = MAX_PATH;
    HANDLE hDumpFile;
    SYSTEMTIME stLocalTime;
    MINIDUMP_EXCEPTION_INFORMATION ExpParam;

	// Dynamic get function
	auto hDbgHelp = LoadLibraryA("dbghelp");
    if(hDbgHelp == NULL)
        return EXCEPTION_EXECUTE_HANDLER;
    auto pMiniDumpWriteDump = (decltype(&MiniDumpWriteDump))GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
    if(pMiniDumpWriteDump == NULL)
        return EXCEPTION_EXECUTE_HANDLER;

    GetLocalTime(&stLocalTime);

	//GetTempPath(dwBufferSize, szPath);
	ExpandEnvironmentStrings(L"%SYSTEMDRIVE%", szPath, MAX_PATH);
	StringCchCat (szPath, MAX_PATH, L"\\TEMP\\");

    StringCchPrintf(szFileName, MAX_PATH, L"%s%s-%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp", 
               szPath, szAppName, szVersion, 
               stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay, 
               stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond, 
               GetCurrentProcessId(), GetCurrentThreadId());
    hDumpFile = CreateFile(szFileName, GENERIC_READ|GENERIC_WRITE, 
                FILE_SHARE_WRITE|FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

    ExpParam.ThreadId = GetCurrentThreadId();
    ExpParam.ExceptionPointers = pExceptionPointers;
    ExpParam.ClientPointers = TRUE;

    bMiniDumpSuccessful = pMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), 
                    hDumpFile, MiniDumpWithDataSegs, &ExpParam, NULL, NULL);

	CloseHandle(hDumpFile);

    return EXCEPTION_EXECUTE_HANDLER;
}

LONG CALLBACK unhandled_handler(EXCEPTION_POINTERS* e)
{
    HANDLE hDumpFile = CreateFile(L"C:\\TEMP\AA", GENERIC_READ|GENERIC_WRITE, 
                FILE_SHARE_WRITE|FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	CloseHandle(hDumpFile);
    GenerateDump(e);
    return EXCEPTION_CONTINUE_SEARCH;
}
#endif //WIN32
#endif //DEBUG

class PluginFactory: public FB::FactoryBase {
public:
	///////////////////////////////////////////////////////////////////////////////
	/// @fn FB::PluginCorePtr createPlugin(const std::string& mimetype)
	///
	/// @brief  Creates a plugin object matching the provided mimetype
	///         If mimetype is empty, returns the default plugin
	///////////////////////////////////////////////////////////////////////////////
	FB::PluginCorePtr createPlugin(const std::string& mimetype) {
		FBLOG_DEBUG("createPlugin()", mimetype);
		if (mimetype == "application/x-linphone-web-video")
			return boost::make_shared<VideoPlugin>();
		return boost::make_shared<CorePlugin>();
	}

	///////////////////////////////////////////////////////////////////////////////
	/// @see FB::FactoryBase::globalPluginInitialize
	///////////////////////////////////////////////////////////////////////////////
	void globalPluginInitialize() {
#ifdef DEBUG
#ifdef WIN32
		AddVectoredExceptionHandler(TRUE, unhandled_handler);
#endif //WIN32
#endif //DEBUG

		srand((unsigned int)time(NULL));

		CorePlugin::StaticInitialize();
		VideoPlugin::StaticInitialize();
	}

	///////////////////////////////////////////////////////////////////////////////
	/// @see FB::FactoryBase::globalPluginDeinitialize
	///////////////////////////////////////////////////////////////////////////////
	void globalPluginDeinitialize() {
		CorePlugin::StaticDeinitialize();
		VideoPlugin::StaticDeinitialize();
	}

	void getLoggingMethods(FB::Log::LogMethodList& outMethods) {
#ifndef WIN32
		// The next line will enable logging to the console (think: printf).
		outMethods.push_back(std::make_pair(FB::Log::LogMethod_Console, std::string()));
#else
		CHAR szPath[MAX_PATH]; 
		CHAR szFileName[MAX_PATH]; 
		CHAR* szAppName = FBSTRING_PluginFileName;
		CHAR* szVersion = FBSTRING_PLUGIN_VERSION;
		DWORD dwBufferSize = MAX_PATH;
		SYSTEMTIME stLocalTime;

		GetLocalTime(&stLocalTime);
		//GetTempPath(dwBufferSize, szPath);
		ExpandEnvironmentStringsA("%SYSTEMDRIVE%", szPath, MAX_PATH);
		StringCchCatA(szPath, MAX_PATH, "\\TEMP\\");

		StringCchPrintfA(szFileName, MAX_PATH, "%s\\%s-%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld_FB.log", 
			szPath, szAppName, szVersion, 
			stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
			stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond,
			GetCurrentProcessId(), GetCurrentThreadId());
		outMethods.push_back(std::make_pair(FB::Log::LogMethod_File, std::string(szFileName)));
#endif
	}

	FB::Log::LogLevel getLogLevel() {
#ifdef DEBUG
		return FB::Log::LogLevel_Debug; // Now Debug and above is logged.
#else
        return FB::Log::LogLevel_Error;
#endif //DEBUG
	}
};

///////////////////////////////////////////////////////////////////////////////
/// @fn getFactoryInstance()
///
/// @brief  Returns the factory instance for this plugin module
///////////////////////////////////////////////////////////////////////////////
FB::FactoryBasePtr getFactoryInstance() {
	static boost::shared_ptr<PluginFactory> factory = boost::make_shared<PluginFactory>();
	return factory;
}

