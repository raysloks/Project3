#pragma once

#include "CustomBehaviour.h"

#include <memory>
#include <functional>

class Collision;

class Player :
	public CustomBehaviour
{
public:
	void tick(float dt);

	std::shared_ptr<std::function<void(const Collision&)>> on_collision;

	std::shared_ptr<std::function<void(void)>> on_attack;
};

