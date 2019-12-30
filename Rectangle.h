#pragma once

#include "Shape.h"

#include "Vec2.h"

class Rectangle :
	public Shape
{
public:
	Rectangle(const Vec2& size);

	bool check(const Vec2& diff, const Shape * other, Collision& collision) const;

	Vec2 size;
};

