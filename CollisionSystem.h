#pragma once

#include "System.h"

#include "Collider.h"

#include "CircleCollider.h"
#include "RectangleCollider.h"
#include "TilemapCollider.h"
#include "SpriteCollider.h"

class CollisionSystem :
	public System
{
public:
	void tick(float dt);

	ComponentContainer<Collider> colliders;

	ComponentContainer<CircleCollider> circles;
	ComponentContainer<RectangleCollider> rectangles;
	ComponentContainer<TilemapCollider> tilemaps;
	ComponentContainer<SpriteCollider> sprites;

	std::map<float, Collider*> overlapCircle(const Vec2& p, float r);
};

