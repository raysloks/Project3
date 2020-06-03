#pragma once

#include "Collider.h"

#include "Sprite.h"

class CircleCollider;

class SpriteCollider :
	public Collider
{
public:
	SpriteCollider();
	SpriteCollider(const Reference<Sprite> & sprite);

	void check(const Vec2 & diff, const CircleCollider & circle, std::vector<Collision> & collisions) const;

	float rotation;
	std::shared_ptr<SpriteSheet> non_iso;
	Reference<Sprite> sprite;
};

