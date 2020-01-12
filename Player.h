#pragma once

#include "Mob.h"

#include <memory>
#include <functional>

class Collision;

class SpriteShape;

class Tilemap;

class Player :
	public Mob
{
public:
	Player();

	void tick(float dt);

	Tilemap * tm;

	void onCollision(const Collision & collision);

	void splatter();

private:

	void onAction(size_t action);

	void update_camera();

	std::function<void(const Collision&)> on_hit;

	std::function<void(void)> on_attack;

	Vec2 facing;

	SpriteShape * sword_shape;
	Reference<Entity> sword;

	bool flip;

	size_t blood;
};

