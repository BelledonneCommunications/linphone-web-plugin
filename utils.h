#ifndef H_utils
#define H_utils

#include <map>
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
		const auto it = std::find_if(threads.begin(), threads.end(),
				[id](const std::pair<boost::thread::id, boost::thread*> &p) -> bool { return p.second->get_id() == id; }
		);
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

#endif // H_utils
