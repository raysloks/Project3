#pragma once

#include "Vec2.h"

class Collider;

class Collision
{
public:
	Vec2 p, n;
	float pen;
	Collider * other;
};

