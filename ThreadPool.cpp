#include "ThreadPool.h"

ThreadPool::ThreadPool()
{
	stopping = false;

	for (size_t i = 0; i < 8; ++i)
	{
		threads.push_back(std::thread([this]()
			{
				std::function<void(void)> function;
				while (true)
				{
					{
						std::unique_lock<std::mutex> lock(mutex);
						condition.wait(lock, [this]() { return !queue.empty() || stopping; });
						function = queue.front();
						queue.pop();
					}
					function();
				}
			}
		));
	}
}

ThreadPool::~ThreadPool()
{
	{
		std::unique_lock<std::mutex> lock(mutex);
		stopping = true;
	}
	condition.notify_all();
	for (auto& thread : threads)
		thread.join();
}

void ThreadPool::push(const std::function<void(void)>& function)
{
	{
		std::unique_lock<std::mutex> lock(mutex);
		queue.push(function);
	}
	condition.notify_one();
}
