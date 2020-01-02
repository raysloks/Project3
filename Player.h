#pragma once

#include "CustomBehaviour.h"

#include <memory>
#include <functional>

class Collision;

class Player :
	public CustomBehaviour
{
public:
	Player();

	void tick(float dt);

private:

	std::shared_ptr<std::function<void(const Collision&)>> on_collision;

	std::shared_ptr<std::function<void(void)>> on_attack;

	Vec2 v;

	float anim = 0.0f;
};

