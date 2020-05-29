#pragma once

#include "Mob.h"

class NetworkMob :
	public Mob
{
public:
	NetworkMob();

	void start();

	void tick(float dt);
};

