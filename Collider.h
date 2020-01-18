#pragma once

#include "Component.h"

#include <vector>
#include <memory>
#include <functional>

#include "Collision.h"

class Collider :
	public Component
{
public:
	Collider();

	uint64_t layers;

	std::vector<std::function<void(const Collision&)>> callbacks;
};

