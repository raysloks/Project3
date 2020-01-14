#pragma once

#include "Mob.h"

#include <memory>
#include <functional>
#include <list>

class Collision;

class SpriteShape;

class Tilemap;

class Player :
	public Mob
{
public:
	Player();

	void start();

	void tick(float dt);

	Tilemap * tm;

	void onCollision(const Collision & collision);

	void splatter();

private:

	void onKey(size_t action);

	bool onAction(size_t action);

	void update_camera();

	std::function<void(const Collision&)> on_hit;

	std::function<void(void)> on_attack;

	Vec2 facing;

	SpriteShape * sword_shape;
	Reference<Entity> sword;

	bool flip;

	size_t blood;

	std::list<std::pair<float, size_t>> key_buffer;
};

