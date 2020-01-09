#pragma once

#include "CustomBehaviour.h"

class Body :
	public CustomBehaviour
{
public:
	void start();
	void tick(float dt);

	Vec2 v;
	float m;
};

