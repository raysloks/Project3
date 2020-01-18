#pragma once

#include "Vec2.h"

class Collider;

class Collision
{
public:

	void flip();

	Vec2 p, n;
	float pen;
	Collider * collider;
	Collider * other;
};

