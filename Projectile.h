#pragma once

#include "CustomBehaviour.h"

class Projectile :
	public CustomBehaviour
{
public:
	void tick(float dt);

	Vec2 v;
};

