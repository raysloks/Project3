#pragma once

#include "Shape.h"

class Circle :
	public Shape
{
public:
	Circle(float r);

	void check(const Vec2& diff, const Shape * other, std::vector<Collision>& collisions) const;

	float r;
};

