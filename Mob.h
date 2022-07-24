#pragma once

#include "CustomBehaviour.h"

#include "MobStatBlock.h"
#include "MobStats.h"

#include "ResourceBar.h"

#include "AbilitySlot.h"

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

	ResourceBar hp;

	Vec2 v, n, move;

	float anim;

	float cooldown;

	MobStatBlock stat_cache;
	MobStats stats;

	std::vector<AbilitySlot> abilities;

	void recalculateStats();

	float getMovementSpeed() const;
};

