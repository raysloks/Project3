#pragma once

#include "Mob.h"

class Enemy :
	public Mob
{
public:
	Enemy();

	void start();

	void tick(float dt);
};

