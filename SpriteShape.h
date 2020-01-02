#pragma once

#include "Shape.h"

#include "Sprite.h"

class SpriteShape :
	public Shape
{
public:
	void check(const Vec2& diff, const Shape * other, std::vector<Collision>& collisions) const;

	Sprite * sprite;
};

