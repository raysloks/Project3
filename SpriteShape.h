#pragma once

#include "Shape.h"

#include "Sprite.h"

#include "Reference.h"

class SpriteShape :
	public Shape
{
public:
	SpriteShape();
	SpriteShape(Reference<Sprite> sprite);

	void check(const Vec2& diff, const Shape * other, std::vector<Collision>& collisions) const;

	std::shared_ptr<SpriteSheet> sheet;
	Reference<Sprite> sprite;
};

