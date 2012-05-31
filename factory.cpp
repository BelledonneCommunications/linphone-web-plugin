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
#include "coreplugin.h"
#include "videoplugin.h"

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
			return boost::make_shared<video>();
		return boost::make_shared<core>();
	}

	///////////////////////////////////////////////////////////////////////////////
	/// @see FB::FactoryBase::globalPluginInitialize
	///////////////////////////////////////////////////////////////////////////////
	void globalPluginInitialize() {
		core::StaticInitialize();
		video::StaticInitialize();
	}

	///////////////////////////////////////////////////////////////////////////////
	/// @see FB::FactoryBase::globalPluginDeinitialize
	///////////////////////////////////////////////////////////////////////////////
	void globalPluginDeinitialize() {
		core::StaticDeinitialize();
		video::StaticDeinitialize();
	}

	void getLoggingMethods(FB::Log::LogMethodList& outMethods) {
		// The next line will enable logging to the console (think: printf).
		outMethods.push_back(std::make_pair(FB::Log::LogMethod_Console, std::string()));
	}

	FB::Log::LogLevel getLogLevel() {
		return FB::Log::LogLevel_Debug; // Now Debug and above is logged.
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

