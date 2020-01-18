#pragma once

#include "Entity.h"

#include <memory>

class Component
{
public:
	virtual ~Component();

	static void attach(const Reference<Component> & component, const Reference<Entity> & entity);
	static void detach(const Reference<Component> & component, const Reference<Entity> & entity);

	template <class T>
	Reference<T> getComponent() const
	{
		return entity->getComponent<T>();
	}

	Reference<Entity> entity;

protected:
	Component();
};

