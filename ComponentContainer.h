#pragma once

#include <vector>
#include <stack>

template <class T>
class Reference;

// BIG TODO delay reallocation to a controlled moment
// we could loop through components via index i guess
// what is the performance impact of that?
// apparently we are already doing that for colliders, which explains why this wasn't a problem before
// for now, we just loop via index to prevent crashes

template <class T>
class ComponentContainer
{
public:
	ComponentContainer();

	template <class... Args>
	Reference<T> add(Args&&... args);
	void remove(const Reference<T> & ref);

	std::vector<T> components;
	std::stack<size_t> vacant;
};

#include "Reference.h"

template<class T>
inline ComponentContainer<T>::ComponentContainer()
{
	//components.reserve(256 * 256);
}

template<class T>
inline void ComponentContainer<T>::remove(const Reference<T> & ref)
{
	vacant.push(ref.offset / sizeof(T));
}

template<class T>
template<class ...Args>
inline Reference<T> ComponentContainer<T>::add(Args && ...args)
{
	if (vacant.empty())
	{
		components.emplace_back(T(args...));
		return Reference<T>(&components, (components.size() - 1) * sizeof(T));
	}
	else
	{
		size_t index = vacant.top();
		vacant.pop();
		components[index] = T(args...);
		return Reference<T>(&components, index * sizeof(T));
	}
}
