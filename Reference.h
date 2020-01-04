#pragma once

template <class T>
class ComponentContainer;

template <class T>
class Reference
{
public:
	Reference();
	Reference(ComponentContainer<T> * container, size_t index);

	ComponentContainer<T> * container;
	size_t index;

	T & operator*() const;
	T * operator->() const;
};

#include "ComponentContainer.h"

template<class T>
inline Reference<T>::Reference() : container(nullptr), index(0)
{
}

template<class T>
inline Reference<T>::Reference(ComponentContainer<T>* container, size_t index) : container(container), index(index)
{
}

template<class T>
inline T & Reference<T>::operator*() const
{
	return container->components[index];
}

template<class T>
inline T * Reference<T>::operator->() const
{
	return &container->components[index];
}
