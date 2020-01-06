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

	std::vector<Tile> tiles;
	size_t w, h;

	Vec2 tile_size;
};

