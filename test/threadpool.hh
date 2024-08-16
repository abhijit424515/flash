#include "common.hh"

struct ThreadPool {
    ThreadPool(size_t numThreads) : stop(false) {
		for (size_t i = 0; i < numThreads; ++i) {
			workers.emplace_back([this] { this->workerThread(); });
		}
	}
    ~ThreadPool() {
		stop = true;
		condition.notify_all();
		for (thread &worker : workers)
			worker.join();
	}

    template<typename F, typename... Args>
	void enqueue(F&& f, Args&&... args) {
		{
			unique_lock<mutex> lock(queueMutex);
			tasks.emplace([f, args...]() { f(args...); });
		}
		condition.notify_one();
	}

private:
    vector<thread> workers;
    queue<function<void()>> tasks;
    
	mutex queueMutex;
    condition_variable condition;
    atomic<bool> stop;

    void workerThread() {
		while (1) {
			function<void()> task;
			{
				unique_lock<mutex> lock(queueMutex);
				condition.wait(lock, [this] { return stop || !tasks.empty(); });
				if (stop && tasks.empty())
					return;
				task = move(tasks.front());
				tasks.pop();
			}
			task();
		}
	}
};