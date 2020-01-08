#pragma once

#include "CustomBehaviour.h"

class Mob :
	public CustomBehaviour
{
public:
	void start();

	void tick(float dt);

	virtual void onCollision(const Collision & collision);

	int64_t hp, hp_max;

	Vec2 v, n, move;

	float speed, acceleration, deceleration;
};

