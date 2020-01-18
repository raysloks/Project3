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
inline void ComponentContainer<T>::remove(size_t index)
{
	vacant.push(index);
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
