#pragma once

#include "System.h"

#include "Collider.h"

class CollisionSystem :
	public System
{
public:
	void tick(float dt);

	ComponentContainer<Collider> colliders;

	std::map<float, Collider*> overlapCircle(const Vec2& p, float r);
};

