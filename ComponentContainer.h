#pragma once

#include <vector>
#include <stack>

template <class T>
class ComponentContainer
{
public:
	ComponentContainer();

	size_t add(T && component);
	void remove(size_t index);

	std::vector<T> components;
	std::stack<size_t> vacant;
};

template<class T>
inline ComponentContainer<T>::ComponentContainer()
{
	components.reserve(256);
}

template<class T>
inline size_t ComponentContainer<T>::add(T && component)
{
	if (vacant.empty())
	{
		components.emplace_back(component);
		return components.size();
	}
	else
	{
		size_t index = vacant.top();
		vacant.pop();
		components[index] = component;
		return index;
	}
}

template<class T>
inline void ComponentContainer<T>::remove(size_t index)
{
	vacant.push(index);
}
