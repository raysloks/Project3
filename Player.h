#pragma once

#include "CustomBehaviour.h"

#include <memory>
#include <functional>

class Collision;

class SpriteShape;

class Tilemap;

class Player :
	public CustomBehaviour
{
public:
	Player();

	void tick(float dt);

	Tilemap * tm;

private:

	void onAction(size_t action);

	void onCollision(const Collision& collision);

	void update_camera();

	bool on_collision;

	std::function<void(const Collision&)> on_hit;

	std::function<void(void)> on_attack;

	Vec2 v;
	
	Vec2 n;

	Vec2 facing;

	float anim = 0.0f;

	SpriteShape * sword_shape;
	Reference<Entity> sword;

	bool flip;

	size_t blood;
};

