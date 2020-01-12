#pragma once

#include "Entity.h"

class Level
{
public:

	Reference<Entity> add_entity(Entity&& entity);
	Entity * get_entity(size_t index);
	void remove_entity(Entity * entity);
	void remove_entity(size_t index);

private:
	ComponentContainer<Entity> entities;
};

