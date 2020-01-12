#pragma once

#include "Collider.h"

#include "RectangleCollider.h"

class CircleCollider;

class Tilemap;

class TilemapCollider :
	public Collider
{
public:
	TilemapCollider(Tilemap * tilemap);

	void checkTile(intmax_t x, intmax_t y, const Vec2& diff, const CircleCollider& circle, std::vector<Collision>& collisions) const;

	void check(const Vec2& diff, const CircleCollider& circle, std::vector<Collision>& collisions) const;

	Tilemap * tilemap;

	RectangleCollider rectangle, rectangle_w, rectangle_h;
};

