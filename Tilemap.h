#pragma once

#include <vector>

#include "Vec2.h"

#include "Tile.h"

class Tilemap
{
public:
	Tilemap(size_t w, size_t h);

	Tile& at(size_t x, size_t y);
	Tile * operator[](size_t x);

	uint8_t getEffect(const Vec2& p);
	void setEffect(const Vec2& p, uint8_t effect);

	std::vector<Tile> tiles;
	size_t w, h;

	Vec2 tile_size;
};

