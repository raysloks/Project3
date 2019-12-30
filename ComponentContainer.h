#pragma once

#include <vector>
#include <stack>

template <class T>
class ComponentContainer
{
public:
	ComponentContainer();

	T * add(T && component);
	void remove(T * pComponent);
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
inline T * ComponentContainer<T>::add(T && component)
{
	if (vacant.empty())
	{
		components.emplace_back(std::move(component));
		return &components.back();
	}
	else
	{
		size_t index = vacant.top();
		vacant.pop();
		components[index] = std::move(component);
		return &components.at(index);
	}
}

template<class T>
inline void ComponentContainer<T>::remove(T * pComponent)
{
	size_t index = std::distance(components.begin(), pComponent);
	vacant.push(index);
}

template<class T>
inline void ComponentContainer<T>::remove(size_t index)
{
	vacant.push(index);
}
