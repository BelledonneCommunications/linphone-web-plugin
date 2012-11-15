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

#ifndef H_UTILS
#define H_UTILS

#include <map>
#include <sstream>
#include <boost/thread.hpp>

class mythread_group {
private:
	mythread_group(mythread_group const&);
	mythread_group& operator=(mythread_group const&);
public:
	mythread_group() {
	}
	~mythread_group() {
		for (auto it = threads.begin(), end = threads.end(); it != end; ++it) {
			delete it->second;
		}
	}

	template<typename F>
	boost::thread* create_thread(F threadfunc) {
		boost::lock_guard<boost::shared_mutex> guard(m);
		boost::thread *thread = new boost::thread(threadfunc);
		threads.insert(std::pair<boost::thread::id, boost::thread*>(thread->get_id(), thread));
		return thread;
	}

	void add_thread(boost::thread* thrd) {
		if (thrd) {
			boost::lock_guard<boost::shared_mutex> guard(m);
			threads.insert(std::pair<boost::thread::id, boost::thread*>(thrd->get_id(), thrd));
		}
	}

	void remove_thread(boost::thread::id id) {
		boost::lock_guard<boost::shared_mutex> guard(m);
		auto it = threads.begin();
		while(it != threads.end()) {
			if(it->second->get_id() == id)
				break;
			++it;
		}
		if (it != threads.end()) {
			delete it->second;
			threads.erase(it);
		}
	}

	void join_all() {
		boost::shared_lock<boost::shared_mutex> guard(m);

		for (auto it = threads.begin(), end = threads.end(); it != end; ++it) {
			(*it).second->join();
		}
	}

	void interrupt_all() {
		boost::shared_lock<boost::shared_mutex> guard(m);

		for (auto it = threads.begin(), end = threads.end(); it != end; ++it) {
			(*it).second->interrupt();
		}
	}

	size_t size() const {
		boost::shared_lock<boost::shared_mutex> guard(m);
		return threads.size();
	}

private:
	std::map<boost::thread::id, boost::thread*> threads;
	mutable boost::shared_mutex m;
};

template<class T>
std::string APIDescription(T *ptr) {
	std::stringstream ss;
	const char *name = typeid(ptr).name();
	if (name != NULL)
		ss << std::string(name);
	else
		ss << "<JSAPI-Auto Javascript Object>";
#ifdef DEBUG
	ss << "(" << ptr << ")";
#endif
	return ss.str();
}

std::string CHARPTR_TO_STRING(const char *str);

#define __DECLARE_SYNC_N_ASYNC_PARAMMACRO(z, n, args) BOOST_PP_ARRAY_ELEM(n, args) p##n
#define __DECLARE_SYNC_N_ASYNC_USEMACRO(z, n, args) p##n

#define REGISTER_SYNC_N_ASYNC(name, funct_name)                                                   \
	registerMethod(name, make_method(this, &CoreAPI::funct_name));                                \
	registerMethod(name "_async", make_method(this, &CoreAPI::BOOST_PP_CAT(funct_name, _async))); \

#define DECLARE_SYNC_N_ASYNC_SYNC_FCT(name, argCount, argList, ret)                               \
	ret name (BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_PARAMMACRO, (argCount, argList)));   \

#define BOOST_MPL_PP_TOKEN_EQUAL_void(x) x

#ifdef CORE_THREADED
#define DECLARE_SYNC_N_ASYNC_THREAD_FCT(name, argCount, argList, ret)                                                                                               \
	BOOST_PP_IF(BOOST_PP_EQUAL(argCount, 0),                                                                                                                        \
			void BOOST_PP_CAT(name, _async_thread) (FB::JSObjectPtr callback) {,                                                                                    \
			void BOOST_PP_CAT(name, _async_thread) (BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_PARAMMACRO, (argCount, argList)), FB::JSObjectPtr callback) {)   \
	BOOST_PP_IF(BOOST_MPL_PP_TOKEN_EQUAL(ret, void),                                                                                                                \
		name(BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_USEMACRO, (argCount, argList)));                                                                        \
		callback->InvokeAsync("", FB::variant_list_of(shared_from_this()));,                                                                                        \
		ret value = name(BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_USEMACRO, (argCount, argList)));                                                            \
		callback->InvokeAsync("", FB::variant_list_of(shared_from_this())(value));)                                                                                 \
		m_threads->remove_thread(boost::this_thread::get_id());                                                                                                     \
	}                                                                                                                                                               \

#define DECLARE_SYNC_N_ASYNC_ASYNC_FCT(name, argCount, argList)                                                                                                     \
	BOOST_PP_IF(BOOST_PP_EQUAL(argCount, 0),                                                                                                                        \
			void BOOST_PP_CAT(name, _async) (FB::JSObjectPtr callback) {,                                                                                           \
			void BOOST_PP_CAT(name, _async) (BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_PARAMMACRO, (argCount, argList)), FB::JSObjectPtr callback) {)          \
		m_threads->create_thread(boost::bind(&CoreAPI::BOOST_PP_CAT(name, _async_thread), this,                                                                     \
		BOOST_PP_IF(BOOST_PP_NOT_EQUAL(argCount, 0), BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_USEMACRO, (argCount, argList)), BOOST_PP_EMPTY())               \
		BOOST_PP_IF(BOOST_PP_NOT_EQUAL(argCount, 0), BOOST_PP_COMMA, BOOST_PP_EMPTY)()                                                                              \
													callback));                                                                                                     \
	}                                                                                                                                                               \

#else
#define DECLARE_SYNC_N_ASYNC_ASYNC_FCT(name, argCount, argList, ret)                                                                                                \
	BOOST_PP_IF(BOOST_PP_EQUAL(argCount, 0),                                                                                                                        \
			void BOOST_PP_CAT(name, _async) (FB::JSObjectPtr callback) {,                                                                                           \
			void BOOST_PP_CAT(name, _async) (BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_PARAMMACRO, (argCount, argList)), FB::JSObjectPtr callback) {)          \
	BOOST_PP_IF(BOOST_MPL_PP_TOKEN_EQUAL(ret, void),                                                                                                                \
		name(BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_USEMACRO, (argCount, argList)));                                                                        \
		callback->InvokeAsync("", FB::variant_list_of(shared_from_this()));,                                                                                        \
		ret value = name(BOOST_PP_ENUM(argCount, __DECLARE_SYNC_N_ASYNC_USEMACRO, (argCount, argList)));                                                            \
		callback->InvokeAsync("", FB::variant_list_of(shared_from_this())(value));)                                                                                 \
	}                                                                                                                                                               \

#endif //CORE_THREADED
#ifdef CORE_THREADED
#define DECLARE_SYNC_N_ASYNC(name, argCount, argList, ret)          \
	DECLARE_SYNC_N_ASYNC_SYNC_FCT(name, argCount, argList, ret)     \
	DECLARE_SYNC_N_ASYNC_THREAD_FCT(name, argCount, argList, ret)   \
	DECLARE_SYNC_N_ASYNC_ASYNC_FCT(name, argCount, argList)         \

#else
#define DECLARE_SYNC_N_ASYNC(name, argCount, argList, ret)          \
	DECLARE_SYNC_N_ASYNC_SYNC_FCT(name, argCount, argList, ret)     \
	DECLARE_SYNC_N_ASYNC_ASYNC_FCT(name, argCount, argList, ret)    \

#endif //CORE_THREADED

#endif // H_UTILS
