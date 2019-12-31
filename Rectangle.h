#pragma once

#include "Shape.h"

#include "Vec2.h"

class Rectangle :
	public Shape
{
public:
	Rectangle(const Vec2& size);

	void check(const Vec2& diff, const Shape * other, std::vector<Collision>& collisions) const;

	Vec2 size;
};

