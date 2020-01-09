#pragma once

#include "Collider.h"

class CircleCollider :
	public Collider
{
public:
	CircleCollider(float r);

	float r;

	void check(const Vec2& diff, const CircleCollider& circle, std::vector<Collision>& collisions) const;
};

