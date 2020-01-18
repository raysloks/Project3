#include "Level.h"

Level::Level() : tilemap(64, 64)
{
}

Reference<Entity> Level::add_entity()
{
	auto entity = entities.add();
	entity->root = entity;
	return entity;
}

void Level::remove_entity(Reference<Entity> entity)
{
	remove_entity_rough(entity);

	Entity::orphan(entity);
}

void Level::remove_entity_rough(const Reference<Entity>& entity)
{
	for (auto child : entity->getChildren())
		remove_entity_rough(child);

	for (auto component : entity->components)
		component->entity = nullptr;

	entities.remove(entity);
}
