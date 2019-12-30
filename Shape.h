#pragma once

#include "Collision.h"

class Shape
{
public:
	virtual bool check(const Vec2& diff, const Shape * other, Collision& collision) const;
};

