#pragma once

#include <type_traits>

#include <vector>

template <class T>
class ComponentContainer;

template <class T>
class Reference
{
public:
	Reference();
	Reference(std::vector<T> * container, size_t offset);
	Reference(std::nullptr_t);

	T & operator*() const;
	T * operator->() const;

	operator bool() const;

	bool operator!=(const Reference<T> & rhs) const;
	bool operator==(const Reference<T> & rhs) const;

	T * get() const;

	template <class U>
	Reference<U> cast() const
	{
		auto p = dynamic_cast<U*>(get());
		if (p)
			return Reference<U>((std::vector<U>*)container, offset);
		else
			return Reference<U>(nullptr);
	}

	template <class U>
	operator Reference<U> & ()
	{
		static_assert(std::is_base_of<U, T>::value, "Cannot upcast this to that.");
		return *(Reference<U>*)this;
	}

private:
	friend class ComponentContainer<T>;

	std::vector<T> * container;
	size_t offset;
};

#include "ComponentContainer.h"

template<class T>
inline Reference<T>::Reference() : container(nullptr), offset(0)
{
}

template<class T>
inline Reference<T>::Reference(std::vector<T> * container, size_t offset) : container(container), offset(offset)
{
}

template<class T>
inline Reference<T>::Reference(std::nullptr_t) : container(nullptr), offset(0)
{
}

template<class T>
inline T & Reference<T>::operator*() const
{
	return *(T*)(((uint8_t*)container->data()) + offset);
}

template<class T>
inline T * Reference<T>::operator->() const
{
	return (T*)(((uint8_t*)container->data()) + offset);
}

template<class T>
inline Reference<T>::operator bool() const
{
	return container;
}

template<class T>
inline bool Reference<T>::operator!=(const Reference<T>& rhs) const
{
	return container != rhs.container || offset != rhs.offset;
}

template<class T>
inline bool Reference<T>::operator==(const Reference<T>& rhs) const
{
	return container == rhs.container && offset == rhs.offset;
}

template<class T>
inline T * Reference<T>::get() const
{
	return (T*)(((uint8_t*)container->data()) + offset);
}