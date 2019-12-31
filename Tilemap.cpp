#include "Tilemap.h"

Tilemap::Tilemap(size_t w, size_t h) : w(w), h(h), tiles(w * h)
{
}

uint32_t & Tilemap::at(size_t x, size_t y)
{
	return tiles.at(x + y * w);
}
