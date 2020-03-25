#pragma once

#include <vector>
#include <stack>

template <class T>
class Reference;


// I'm running out of time and need to fix the last memory leak
class BaseContainer
{
public:
	virtual ~BaseContainer() {}

	virtual size_t move(size_t offset, BaseContainer * destination) = 0;
};


template <class T>
class ComponentContainer :
	public BaseContainer
{
public:
	ComponentContainer();

	template <class... Args>
	Reference<T> add(Args&&... args);
	void remove(const Reference<T> & ref);
	void remove(size_t index);

	size_t move(size_t offset, BaseContainer * destination);

	std::vector<T> components;
	std::stack<size_t> vacant;
};

#include "Reference.h"

template<class T>
inline ComponentContainer<T>::ComponentContainer()
{
	components.reserve(64 * 64); // comment out when testing reallocation
}

template<class T>
inline void ComponentContainer<T>::remove(const Reference<T> & ref)
{
	vacant.push(ref.offset / sizeof(T));
}

template<class T>
inline void ComponentContainer<T>::remove(size_t index)
{
	vacant.push(index);
}

template<class T>
inline size_t ComponentContainer<T>::move(size_t offset, BaseContainer * destination)
{
	auto dst = dynamic_cast<ComponentContainer<T>*>(destination);
	return dst->add(std::move(components[offset / sizeof(T)])).offset;
}

template<class T>
template<class ...Args>
inline Reference<T> ComponentContainer<T>::add(Args && ...args)
{
	if (vacant.empty())
	{
		components.emplace_back(T(args...));
		return Reference<T>(this, (components.size() - 1) * sizeof(T));
	}
	else
	{
		size_t index = vacant.top();
		vacant.pop();
		components[index] = T(args...);
		return Reference<T>(this, index * sizeof(T));
	}
}
