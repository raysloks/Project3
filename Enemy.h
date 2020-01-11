#pragma once

#include "Mob.h"

class Enemy :
	public Mob
{
public:
	Enemy();

	void tick(float dt);
};

