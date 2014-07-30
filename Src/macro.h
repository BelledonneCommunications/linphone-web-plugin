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

#ifndef H_MACRO
#define H_MACRO

#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/array/elem.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/debug/assert.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/mpl/aux_/preprocessor/token_equal.hpp>


//
// PROPERTY FILE
//
#define DECLARE_PROPERTY_N_DOWNLOAD_FILE_SETTER(class, setter)                  \
	void BOOST_PP_CAT(setter, _file)(StringPtr const &arg);                     \

#define DECLARE_PROPERTY_N_DOWNLOAD_FILE_GETTER(class, getter)                  \
	StringPtr BOOST_PP_CAT(getter, _file)() const;                              \

#define REGISTER_PROPERTY_FILE(class, name, getter, setter)                                                                \
	registerProperty(name, make_property(this, &class::BOOST_PP_CAT(getter, _file), &class::BOOST_PP_CAT(setter, _file))); \


#define DECLARE_PROPERTY_FILE(class, getter, setter)           \
	StringPtr getter() const;                                  \
	void setter(StringPtr const &arg);                         \
	DECLARE_PROPERTY_N_DOWNLOAD_FILE_GETTER(class, getter)     \
	DECLARE_PROPERTY_N_DOWNLOAD_FILE_SETTER(class, setter)     \


#define IMPLEMENT_PROPERTY_N_DOWNLOAD_FILE_SETTER(class, setter)                               \
	void class::BOOST_PP_CAT(setter, _file)(StringPtr const &arg) {                            \
		StringPtr file;                                                                        \
		if(arg) {                                                                              \
			FB::URI uri(*arg);                                                                 \
			file = getFactory()->getFileManager()->uriToFile(uri);                             \
			if(file->empty()) {                                                                \
				FBLOG_WARN(#class "::" #setter "_file", "Invalid file URL: " << arg);          \
				throw FB::script_error(std::string("Invalid file URL: ") + PRINT_STRING(arg)); \
			}                                                                                  \
		}                                                                                      \
		setter(file);                                                                          \
	}                                                                                          \

#define IMPLEMENT_PROPERTY_N_DOWNLOAD_FILE_GETTER(class, getter)                  \
	StringPtr class::BOOST_PP_CAT(getter, _file)() const {                        \
		StringPtr ret = getter();                                                 \
		if(ret) {                                                                 \
			FB::URI uri = getFactory()->getFileManager()->fileToUri(*ret);        \
			if(FileManagerAPI::isFile(uri)) {                                     \
				ret = uri.toString();                                             \
			} else {                                                              \
				FBLOG_WARN(#class "::" #getter "_file", "Invalid file: " << ret); \
			}                                                                     \
		}                                                                         \
		return ret;                                                               \
	}                                                                             \

#define GET_FILE(function, arg)                                                             \
	if(arg) {                                                                               \
		FB::URI uri(*arg);                                                                  \
		arg = getFactory()->getFileManager()->uriToFile(uri);                               \
		if(arg->empty()) {                                                                  \
			FBLOG_WARN(function, "Invalid file URL: " << arg);                              \
			throw FB::script_error(std::string("Invalid file URL: ") + PRINT_STRING(arg));  \
		}                                                                                   \
	}                                                                                       \

#define IMPLEMENT_PROPERTY_FILE(class, getter, setter)         \
	IMPLEMENT_PROPERTY_N_DOWNLOAD_FILE_GETTER(class, getter)   \
	IMPLEMENT_PROPERTY_N_DOWNLOAD_FILE_SETTER(class, setter)   \


#define FB_ASSERT_MSG(x, msg) {if(!(x)) throw FB::script_error("ASSERT ERROR: " msg);}
#define FB_ASSERT(x) FB_ASSERT_MSG(x,BOOST_PP_STRINGIZE(x))
#define FB_ASSERT_CORE FB_ASSERT_MSG(mCore!=NULL, "Core not initialized")

#define CORE_MUTEX boost::recursive_mutex::scoped_lock scopedLock(getFactory()->getCoreMutex(), boost::defer_lock); \
	if(!isOwned()) scopedLock.lock();

#endif // H_MACRO

