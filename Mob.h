#pragma once

#include "CustomBehaviour.h"

#include "MobStatBlock.h"

class Mob :
	public CustomBehaviour
{
public:
	Mob();

	void start();

	void tick(float dt);

	virtual void onCollision(const Collision & collision);

	virtual void onDamaged(int64_t damage);

	uint32_t team;

	int64_t hp, hp_max;

	Vec2 v, n, move;

	float anim;

	float cooldown;

	MobStatBlock stats, base_stats;

	void recalculateStats();
};

