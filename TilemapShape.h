#pragma once

#include "Shape.h"

#include "Tilemap.h"

#include "Rectangle.h"

class TilemapShape :
	public Shape
{
public:
	TilemapShape(Tilemap * tilemap);

	void checkTile(int x, int y, const Vec2& diff, const Shape * other, std::vector<Collision>& collisions) const;

	void check(const Vec2& diff, const Shape * other, std::vector<Collision>& collisions) const;

	Tilemap * tilemap;

	Rectangle rectangle;
};

