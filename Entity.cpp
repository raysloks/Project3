#include "Entity.h"

#include "Component.h"

uint64_t global_counter = 0;

Entity::Entity() : p()
{
	guid = ++global_counter;
}

Entity::Entity(Entity && entity) noexcept
{
	guid = entity.guid;
	entity.guid = 0;
	p = entity.p;
	components = std::move(entity.components);
	for (auto component : components)
		component.second->entity = this;
}

Entity::~Entity()
{
	for (auto component : components)
		component.second->entity = nullptr;
}

Entity & Entity::operator=(Entity && entity) noexcept
{
	guid = entity.guid;
	p = entity.p;
	components = std::move(entity.components);
	for (auto component : components)
		component.second->entity = this;
	return *this;
}

void Entity::componentMoved(Component * pOld, Component * pNew)
{
	for (auto& i : components)
	{
		if (i.second == pOld)
		{
			i.second = pNew;
			return;
		}
	}
}

void Entity::removeComponent(Component * component)
{
	for (auto& i : components)
	{
		if (i.second == component)
		{
			i.second->entity = nullptr;
			components.erase(i.first);
			return;
		}
	}
}
