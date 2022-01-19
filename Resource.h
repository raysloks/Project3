#pragma once

#include <memory>
#include <string>
#include <map>
#include <vector>
#include <functional>

#include "ThreadPool.h"

class BaseResource
{
public:
	static ThreadPool thread_pool;
};

template <class T>
class Resource :
	public std::enable_shared_from_this<T>,
	public BaseResource
{
public:
	static std::map<std::string, std::weak_ptr<T>> resources;

	static std::shared_ptr<T> get(const std::string& fname)
	{
		std::shared_ptr<T> resource = nullptr;
		auto i = resources.find(fname);
		if (i != resources.end())
			resource = i->second.lock();
		if (resource)
			return resource;
		resource = T::load(fname);
		resources[fname] = resource;
		return resource;
	}

	bool loaded = false;
};

template <class T>
std::map<std::string, std::weak_ptr<T>> Resource<T>::resources;

