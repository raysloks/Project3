#pragma once

#include <cstdint>

#include "Position.h"

#include <vector>

class Component;

class Entity
{
public:
	uint64_t id;

	Position p;

	std::vector<Component*> components;
};

