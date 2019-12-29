#pragma once

#include <memory>
#include <string>
#include <map>

template <class T>
class Resource
{
public:

	static std::map<std::string, std::weak_ptr<T>> loaded;

	static std::shared_ptr<T> get(const std::string& fname)
	{
		std::shared_ptr<T> resource = nullptr;
		auto i = loaded.find(fname);
		if (i != loaded.end())
			resource = i->second.lock();
		if (resource)
			return resource;
		resource = std::make_shared<T>(fname);
		loaded[fname] = resource;
		return resource;
	}

};

template <class T>
std::map<std::string, std::weak_ptr<T>> Resource<T>::loaded;

