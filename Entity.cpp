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
		component->entity = this;
}

void Entity::addComponent(Component * component)
{
	component->entity = this;
	components.push_back(component);
}
