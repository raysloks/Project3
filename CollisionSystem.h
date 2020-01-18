#pragma once

#include "System.h"

#include <functional>

class Collider;

class CollisionSystem :
	public System
{
public:
	void tick(float dt);

	std::map<float, Collider*> overlapCircle(const Vec2& p, float r, uint64_t layers = -1, const std::function<bool(Collider*)>& filter = nullptr);
};

