#pragma once

#include "Shape.h"

class Circle :
	public Shape
{
public:
	Circle(float r);

	bool check(const Vec2& diff, const Shape * other, Collision& collision) const;

	float r;
};

