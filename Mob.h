#pragma once

#include "CustomBehaviour.h"

class Mob :
	public CustomBehaviour
{
public:
	Mob();

	void start();

	void tick(float dt);

	virtual void onCollision(const Collision & collision);

	virtual void onDamaged(int64_t damage);

	int64_t hp, hp_max;

	Vec2 v, n, move;

	float speed, acceleration, deceleration;

	float anim;

	float cooldown;
};

