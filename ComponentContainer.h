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

	Reference<T> add(T && component);
	void remove(T * pComponent);
	void remove(size_t index);
	void remove(Reference<T> ref);

	size_t get_index(T * pComponent);
	Reference<T> get_ref(T * pComponent);
	Reference<T> get_ref(size_t index);

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
inline Reference<T> ComponentContainer<T>::add(T && component)
{
	if (vacant.empty())
	{
		components.emplace_back(std::move(component));
		return Reference<T>(this, components.size() - 1);
	}
	else
	{
		size_t index = vacant.top();
		vacant.pop();
		components[index] = std::move(component);
		return Reference<T>(this, index);
	}
}

template<class T>
inline void ComponentContainer<T>::remove(T * pComponent)
{
	size_t index = std::distance(&*components.begin(), pComponent);
	vacant.push(index);
}

template<class T>
inline void ComponentContainer<T>::remove(size_t index)
{
	vacant.push(index);
}

template<class T>
inline void ComponentContainer<T>::remove(Reference<T> ref)
{
	vacant.push(ref.index);
}

template<class T>
inline size_t ComponentContainer<T>::get_index(T * pComponent)
{
	return pComponent - &components[0];
}

template<class T>
inline Reference<T> ComponentContainer<T>::get_ref(T * pComponent)
{
	return Reference<T>(this, pComponent - &components[0]);
}

template<class T>
inline Reference<T> ComponentContainer<T>::get_ref(size_t index)
{
	return Reference<T>(this, index);
}
