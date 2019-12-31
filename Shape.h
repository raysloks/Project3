#pragma once

#include "Collision.h"

#include <vector>

class Shape
{
public:
	virtual void check(const Vec2& diff, const Shape * other, std::vector<Collision>& collisions) const;
};

