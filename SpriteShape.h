#pragma once

#include "Shape.h"

class SpriteShape :
	public Shape
{
public:
	void check(const Vec2& diff, const Shape * other, std::vector<Collision>& collisions) const;
};

