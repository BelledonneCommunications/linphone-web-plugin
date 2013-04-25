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
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

class ThreadGroup {
private:
	ThreadGroup(ThreadGroup const&);
	ThreadGroup& operator=(ThreadGroup const&);
public:
	ThreadGroup() {
	}
	~ThreadGroup() {
	}

	template<typename F>
	boost::shared_ptr<boost::thread> create_thread(F threadfunc) {
		boost::lock_guard<boost::shared_mutex> guard(mMutex);
		boost::shared_ptr<boost::thread> thread = boost::make_shared<boost::thread>(threadfunc);
		mThreadsMap.insert(std::pair<boost::thread::id, boost::shared_ptr<boost::thread> >(thread->get_id(), thread));
		return thread;
	}

	void addThread(const boost::shared_ptr<boost::thread> &thrd) {
		if (thrd) {
			boost::lock_guard<boost::shared_mutex> guard(mMutex);
			mThreadsMap.insert(std::pair<boost::thread::id, boost::shared_ptr<boost::thread> >(thrd->get_id(), thrd));
		}
	}

	void removeThread(boost::thread::id id) {
		boost::lock_guard<boost::shared_mutex> guard(mMutex);
		auto it = mThreadsMap.begin();
		while(it != mThreadsMap.end()) {
			if(it->second->get_id() == id)
				break;
			++it;
		}
		if (it != mThreadsMap.end()) {
			mThreadsMap.erase(it);
		}
	}

	void joinAll() {
		boost::shared_lock<boost::shared_mutex> guard(mMutex);

		for (auto it = mThreadsMap.begin(), end = mThreadsMap.end(); it != end; ++it) {
			(*it).second->join();
		}
	}

	void interruptAll() {
		boost::shared_lock<boost::shared_mutex> guard(mMutex);

		for (auto it = mThreadsMap.begin(), end = mThreadsMap.end(); it != end; ++it) {
			(*it).second->interrupt();
		}
	}

	size_t size() const {
		boost::shared_lock<boost::shared_mutex> guard(mMutex);
		return mThreadsMap.size();
	}

private:
	std::map<boost::thread::id, boost::shared_ptr<boost::thread> > mThreadsMap;
	mutable boost::shared_mutex mMutex;
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

#endif // H_UTILS

