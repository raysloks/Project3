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

	void update_followers();

	std::shared_ptr<std::function<void(const Collision&)>> on_collision;

	std::shared_ptr<std::function<void(void)>> on_attack;

	Vec2 v;
	
	Vec2 n;

	Vec2 facing;

	float anim = 0.0f;

	bool flip;

	size_t shadow;
};

