#pragma once

#include <vector>

#include "Vec2.h"

class Tilemap
{
public:
	Tilemap(size_t w, size_t h);

	uint32_t& at(size_t x, size_t y);
	uint32_t * operator[](size_t x);

	std::vector<uint32_t> tiles;
	size_t w, h;

	Vec2 tile_size;
};

