#pragma once

#include "Mob.h"

#include <memory>
#include <functional>
#include <list>

#include "AbilitySlot.h"

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

	void onCollision(const Collision & collision);

	void splatter();
	
	std::vector<AbilitySlot> abilities;

private:

	void onKey(size_t action);

	bool onAction(size_t action);

	void update_camera();

	void moveToCursor();

	std::function<void(const Collision&)> on_hit;

	std::function<void(void)> on_attack;

	Vec2 facing;

	Reference<Entity> sword;

	bool flip;

	size_t blood;

	std::list<std::pair<float, size_t>> key_buffer;

	Vec2 move_target;
};

