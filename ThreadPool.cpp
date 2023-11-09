#include "ThreadPool.h"

ThreadPool::ThreadPool()
{
	stopping = true;
}

ThreadPool::ThreadPool(size_t size)
{
	stopping = false;

	if (size == 1)
	{
		threads.push_back(std::thread([this]()
			{
				while (true)
				{
					std::unique_lock<std::mutex> lock(mutex);
					if (stopping)
						return;
					if (!queue.empty())
					{
						std::function<void(void)> function = queue.front();
						queue.pop();
						lock.unlock();
						function();
					}
				}
			}
		));
	}
	else
	{
		for (size_t i = 0; i < size; ++i)
		{
			threads.push_back(std::thread([this]()
				{
					while (true)
					{
						std::unique_lock<std::mutex> lock(mutex);
						condition.wait(lock, [this]() { return !queue.empty() || stopping; });
						if (stopping)
							return;
						std::function<void(void)> function = queue.front();
						queue.pop();
						lock.unlock();
						function();
					}
				}
			));
		}
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
