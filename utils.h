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
		const auto it = std::find_if(threads.begin(), threads.end(), [id](const std::pair<boost::thread::id, boost::thread*> &p) -> bool {return p.second->get_id() == id;});
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
#ifndef NDEBUG
	ss << "(" << ptr << ")";
#endif
	return ss.str();
}

static std::string CHARPTR_TO_STRING(const char *str) {
	return (str != NULL)? std::string(str): std::string();
}

#endif // H_UTILS
