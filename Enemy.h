#pragma once

#include "Mob.h"

class Enemy :
	public Mob
{
public:
	void tick(float dt);
};

