/**********************************************************\ 
 
 Auto-generated Factory.cpp
 
 This file contains the auto-generated factory methods 
 for the linphone project
 
 \**********************************************************/

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
		if (mimetype == "application/x-linphone-video")
			return boost::make_shared<video>();
		return boost::make_shared<linphone>();
	}

	///////////////////////////////////////////////////////////////////////////////
	/// @see FB::FactoryBase::globalPluginInitialize
	///////////////////////////////////////////////////////////////////////////////
	void globalPluginInitialize() {
		linphone::StaticInitialize();
	}

	///////////////////////////////////////////////////////////////////////////////
	/// @see FB::FactoryBase::globalPluginDeinitialize
	///////////////////////////////////////////////////////////////////////////////
	void globalPluginDeinitialize() {
		linphone::StaticDeinitialize();
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

