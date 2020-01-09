#pragma once

#include "Entity.h"

class Component
{
public:
	Component();
	Component(Component && component) noexcept;

	Component & operator=(Component && component) noexcept;

	template <class T>
	T * getComponent() const
	{
		return entity->getComponent<T>();
	}

	Entity * entity;
};

