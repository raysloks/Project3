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

Reference<Entity> Level::move_entity(Reference<Entity> entity, Level * destination)
{
	auto entity_new = destination->entities.add(*entity);
	for (auto& component : entity_new->components)
	{
		auto casted = component.cast<CustomBehaviour>();
		if (casted)
		{
			auto i = std::find(custom_behaviours.components.begin(), custom_behaviours.components.end(), casted.container);
			destination->custom_behaviours.add(*i);
			custom_behaviours.remove(i - custom_behaviours.components.begin());
			*i = nullptr;
		}
		else
		{
			BaseContainer * container_new = (BaseContainer*)((uint8_t*)component.container + ((uint8_t*)destination - (uint8_t*)this));
			size_t offset_new = component.container->move(component.offset, container_new);

			component->entity = nullptr;

			component.container = (ComponentContainer<Component>*)container_new;
			component.offset = offset_new;
		}

		component->entity = entity_new;
	}

	for (size_t i = 0; i < entity_new->children.size(); ++i)
	{
		auto temp = move_entity(entity_new->children[i], destination);
		entity_new->children[i] = temp;
		entity_new->children[i]->parent = entity_new;
	}

	return entity_new;
}

void Level::remove_entity_rough(const Reference<Entity>& entity)
{
	for (auto child : entity->getChildren())
		remove_entity_rough(child);

	for (auto component : entity->components)
		component->entity = nullptr;

	entities.remove(entity);
}
