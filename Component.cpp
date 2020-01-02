#include "Component.h"

Component::Component()
{
	entity = nullptr;
}

Component::Component(Component && component) noexcept
{
	entity = component.entity;
	if (entity)
	{
		entity->componentMoved(&component, this);
		component.entity = nullptr;
	}
}

Component & Component::operator=(Component && component) noexcept
{
	if (entity)
		entity->removeComponent(this);
	entity = component.entity;
	if (entity)
	{
		entity->componentMoved(&component, this);
		component.entity = nullptr;
	}
	return *this;
}
