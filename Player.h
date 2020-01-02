#pragma once

#include "CustomBehaviour.h"

#include <memory>
#include <functional>

class Collision;

class SpriteShape;

class Player :
	public CustomBehaviour
{
public:
	Player();

	void tick(float dt);

private:

	void update_followers();

	std::function<void(const Collision&)> on_collision, on_hit;

	std::function<void(void)> on_attack;

	Vec2 v;
	
	Vec2 n;

	Vec2 facing;

	float anim = 0.0f;

	SpriteShape * sword_shape;
	size_t sword = -1;

	bool flip;
};

