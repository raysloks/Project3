#include "Entity.h"

#include "Component.h"

uint64_t global_counter = 0;

Entity::Entity()
{
	guid = ++global_counter;
}

Entity::Entity(Entity && entity) noexcept
{
	guid = entity.guid;
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
