#pragma once

#include "Mob.h"

class MobTemplate;

class NetworkMob :
	public Mob
{
public:
	NetworkMob();

	void start();

	void tick(float dt);

	uint64_t id;

	void setMobTemplate(uint64_t mob_template_id);

	uint64_t mob_template_id;
	const MobTemplate * mob_template;

	float angle;
};

