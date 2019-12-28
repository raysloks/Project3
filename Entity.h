#pragma once

#include <cstdint>

#include "Position.h"

#include <vector>

class Component;

class Entity
{
public:
	Entity();
	Entity(Entity && entity) noexcept;

	void addComponent(Component * component);

	uint64_t guid;

	Position p;

	std::vector<Component*> components;
};

