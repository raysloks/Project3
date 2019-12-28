#pragma once

#include "System.h"

#include "Collider.h"

class CollisionSystem :
	public System
{
public:
	void tick(float dt);

	ComponentContainer<Collider> colliders;
};

