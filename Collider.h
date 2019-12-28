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
	float r;

	std::vector<std::weak_ptr<std::function<void(const Collision&)>>> callbacks;
};

