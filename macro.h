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

#ifndef H_MACRO
#define H_MACRO

#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/array/elem.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/debug/assert.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/mpl/aux_/preprocessor/token_equal.hpp>

#define __DECLARE_SYNC_N_ASYNC_PARAMMACRO(z, n, data) BOOST_PP_ARRAY_ELEM(n, data) paramater_##n
#define __DECLARE_SYNC_N_ASYNC_USEMACRO(z, n, data) paramater_##n

#define REGISTER_SYNC_N_ASYNC(class, name, funct_name)                                            \
	registerMethod(name, make_method(this, &class::funct_name));                                  \
	registerMethod(name "_async", make_method(this, &class::BOOST_PP_CAT(funct_name, _async)));   \

#define DECLARE_SYNC_N_ASYNC_SYNC_FCT(class, name, argCount, argList, ret)                        \
	ret name (BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_PARAMMACRO, (argCount, argList)));   \

#ifdef CORE_THREADED
#define DECLARE_SYNC_N_ASYNC_THREAD_FCT(class, name, argCount, argList, ret)                                                                                        \
	BOOST_PP_IF(BOOST_PP_EQUAL(argCount, 0),                                                                                                                        \
			void BOOST_PP_CAT(name, _async_thread) (boost::optional<FB::JSObjectPtr> callback);,                                                                    \
			void BOOST_PP_CAT(name, _async_thread) (BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_PARAMMACRO, (argCount, argList)),                                \
													boost::optional<FB::JSObjectPtr> callback);)                                                                    \

#define DECLARE_SYNC_N_ASYNC_ASYNC_FCT(class, name, argCount, argList)                                                                                              \
	BOOST_PP_IF(BOOST_PP_EQUAL(argCount, 0),                                                                                                                        \
			void BOOST_PP_CAT(name, _async) (boost::optional<FB::JSObjectPtr> callback);,                                                                           \
			void BOOST_PP_CAT(name, _async) (BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_PARAMMACRO, (argCount, argList)),                                       \
											boost::optional<FB::JSObjectPtr> callback);)                                                                            \


#else
#define DECLARE_SYNC_N_ASYNC_ASYNC_FCT(class, name, argCount, argList, ret)                                                                                         \
	BOOST_PP_IF(BOOST_PP_EQUAL(argCount, 0),                                                                                                                        \
			void BOOST_PP_CAT(name, _async) (boost::optional<FB::JSObjectPtr> callback);,                                                                           \
			void BOOST_PP_CAT(name, _async) (BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_PARAMMACRO, (argCount, argList)),                                       \
											boost::optional<FB::JSObjectPtr> callback);)                                                                            \

#endif //CORE_THREADED

#ifdef CORE_THREADED
#define IMPLEMENT_SYNC_N_ASYNC_THREAD_FCT(class, name, argCount, argList, ret)                                                                                      \
	BOOST_PP_IF(BOOST_PP_EQUAL(argCount, 0),                                                                                                                        \
			void class::BOOST_PP_CAT(name, _async_thread) (boost::optional<FB::JSObjectPtr> callback) {,                                                            \
			void class::BOOST_PP_CAT(name, _async_thread) (BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_PARAMMACRO, (argCount, argList)),                         \
															boost::optional<FB::JSObjectPtr> callback) {)                                                           \
	try {BOOST_PP_IF(BOOST_MPL_PP_TOKEN_EQUAL(ret, void),                                                                                                           \
		name(BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_USEMACRO, NULL));                                                                                       \
		if(callback)(*callback)->InvokeAsync("", FB::variant_list_of(shared_from_this()));,                                                                         \
		ret value = name(BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_USEMACRO, NULL));                                                                           \
		if(callback)(*callback)->InvokeAsync("", FB::variant_list_of(shared_from_this())(value));)                                                                  \
		detachThread(boost::this_thread::get_id());                                                                                                                 \
		} catch(std::runtime_error&) {}                                                                                                                             \
	}                                                                                                                                                               \

#define IMPLEMENT_SYNC_N_ASYNC_ASYNC_FCT(class, name, argCount, argList)                                                                                            \
	BOOST_PP_IF(BOOST_PP_EQUAL(argCount, 0),                                                                                                                        \
			void class::BOOST_PP_CAT(name, _async) (boost::optional<FB::JSObjectPtr> callback) {,                                                                   \
			void class::BOOST_PP_CAT(name, _async) (BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_PARAMMACRO, (argCount, argList)),                                \
													boost::optional<FB::JSObjectPtr> callback) {)                                                                   \
		attachThread(boost::make_shared<boost::thread>(boost::bind(&class::BOOST_PP_CAT(name, _async_thread), this,                                                 \
		BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_USEMACRO, NULL)                                                                                              \
		BOOST_PP_IF(BOOST_PP_NOT_EQUAL(argCount, 0), BOOST_PP_COMMA, BOOST_PP_EMPTY)()                                                                              \
		callback)));                                                                                                                                                \
	}                                                                                                                                                               \

#else
#define IMPLEMENT_SYNC_N_ASYNC_ASYNC_FCT(class, name, argCount, argList, ret)                                                                                       \
	BOOST_PP_IF(BOOST_PP_EQUAL(argCount, 0),                                                                                                                        \
			void class::BOOST_PP_CAT(name, _async) (boost::optional<FB::JSObjectPtr> callback) {,                                                                   \
			void class::BOOST_PP_CAT(name, _async) (BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_PARAMMACRO, (argCount, argList)),                                \
													boost::optional<FB::JSObjectPtr callback>) {)                                                                   \
	try {BOOST_PP_IF(BOOST_MPL_PP_TOKEN_EQUAL(ret, void),                                                                                                           \
		name(BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_USEMACRO, (argCount, argList)));                                                                        \
		if(callback)(*callback)->InvokeAsync("", FB::variant_list_of(shared_from_this()));,                                                                         \
		ret value = name(BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_USEMACRO, (argCount, argList)));                                                            \
		if(callback)(*callback)->InvokeAsync("", FB::variant_list_of(shared_from_this())(value));)                                                                  \
		} catch(std::runtime_error&) {}                                                                                                                             \
	}                                                                                                                                                               \

#endif //CORE_THREADED

#ifdef CORE_THREADED
#define DECLARE_SYNC_N_ASYNC(class, name, argCount, argList, ret)          \
	DECLARE_SYNC_N_ASYNC_SYNC_FCT(class, name, argCount, argList, ret)     \
	DECLARE_SYNC_N_ASYNC_THREAD_FCT(class, name, argCount, argList, ret)   \
	DECLARE_SYNC_N_ASYNC_ASYNC_FCT(class, name, argCount, argList)         \

#else
#define DECLARE_SYNC_N_ASYNC(class, name, argCount, argList, ret)          \
	DECLARE_SYNC_N_ASYNC_SYNC_FCT(class, name, argCount, argList, ret)     \
	DECLARE_SYNC_N_ASYNC_ASYNC_FCT(class, name, argCount, argList, ret)    \

#endif //CORE_THREADED

#ifdef CORE_THREADED
#define IMPLEMENT_SYNC_N_ASYNC(class, name, argCount, argList, ret)          \
	IMPLEMENT_SYNC_N_ASYNC_THREAD_FCT(class, name, argCount, argList, ret)   \
	IMPLEMENT_SYNC_N_ASYNC_ASYNC_FCT(class, name, argCount, argList)         \

#else
#define IMPLEMENT_SYNC_N_ASYNC(class, name, argCount, argList, ret)          \
	IMPLEMENT_SYNC_N_ASYNC_ASYNC_FCT(class, name, argCount, argList, ret)    \

#endif //CORE_THREADED


//
// PROPERTY FILE
//
#define DECLARE_PROPERTY_N_DOWNLOAD_FILE_SETTER(class, setter)                  \
	void BOOST_PP_CAT(setter, _file)(const StringPtr &arg);                     \

#define DECLARE_PROPERTY_N_DOWNLOAD_FILE_GETTER(class, getter)                  \
	StringPtr BOOST_PP_CAT(getter, _file)() const;                              \

#define REGISTER_PROPERTY_FILE(class, name, getter, setter)                                                                \
	registerProperty(name, make_property(this, &class::BOOST_PP_CAT(getter, _file), &class::BOOST_PP_CAT(setter, _file))); \


#define DECLARE_PROPERTY_FILE(class, getter, setter)           \
	StringPtr getter() const;                                  \
	void setter(const StringPtr &arg);                         \
	DECLARE_PROPERTY_N_DOWNLOAD_FILE_GETTER(class, getter)     \
	DECLARE_PROPERTY_N_DOWNLOAD_FILE_SETTER(class, setter)     \


#define IMPLEMENT_PROPERTY_N_DOWNLOAD_FILE_SETTER(class, setter)                               \
	void class::BOOST_PP_CAT(setter, _file)(const StringPtr &arg) {                            \
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


#define FB_ASSERT_MSG(x, msg) {if(!(x)) throw FB::script_error("ASSERT ERROR: "msg);}
#define FB_ASSERT(x) FB_ASSERT_MSG(x,BOOST_PP_STRINGIZE(x))
#define FB_ASSERT_CORE FB_ASSERT_MSG(mCore!=NULL, "Core not initialized")

#ifdef CORE_THREADED
#define CORE_MUTEX boost::recursive_mutex::scoped_lock scopedLock(getFactory()->getCoreMutex(), boost::defer_lock); \
	if(!isOwned()) scopedLock.lock();
#else
#define CORE_MUTEX
#endif //CORE_THREADED

#endif // H_MACRO

