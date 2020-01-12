#include "Level.h"

Reference<Entity> Level::add_entity(Entity && entity)
{
	return entities.add(std::move(entity));
}

Entity * Level::get_entity(size_t index)
{
	if (index > entities.components.size())
		return nullptr;
	return &entities.components[index];
}

void Level::remove_entity(Entity * entity)
{
	for (auto child : entity->getChildren())
		remove_entity(child);
	*entity = std::move(Entity());
	entities.remove(entity);
}

void Level::remove_entity(size_t index)
{
	remove_entity(&entities.components[index]);
}
