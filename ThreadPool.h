#pragma once

#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

class ThreadPool
{
public:
	ThreadPool();
	~ThreadPool();

	void push(const std::function<void(void)>& function);

private:
	std::vector<std::thread> threads;
	std::mutex mutex;
	std::queue<std::function<void(void)>> queue;
	std::condition_variable condition;
	bool stopping;
};

