#pragma once

#include "Collider.h"

class CircleCollider;

class RectangleCollider :
	public Collider
{
public:
	RectangleCollider();
	RectangleCollider(const Vec2& half_size);

	Vec2 half_size;

	void check(const Vec2& diff, const CircleCollider& circle, std::vector<Collision>& collisions) const;
};

