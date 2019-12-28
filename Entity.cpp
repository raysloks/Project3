#include "Entity.h"

#include "Component.h"

Entity::Entity()
{
}

Entity::Entity(Entity && entity) noexcept
{
	guid = entity.guid;
	p = entity.p;
	components = std::move(entity.components);
	for (auto component : components)
		component.second->entity = this;
}
