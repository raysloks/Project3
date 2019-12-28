#pragma once

#include "System.h"

#include "Collider.h"

class CollisionSystem :
	public System
{
public:
	void tick(float dt);

	ComponentContainer<Collider> colliders;

	std::map<float, Collider*> overlapCircle(Vec2 p, float r);
};

