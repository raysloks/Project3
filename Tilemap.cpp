#include "Tilemap.h"

Tilemap::Tilemap(size_t w, size_t h) : w(w), h(h), tiles(w * h)
{
}

Tile & Tilemap::at(size_t x, size_t y)
{
	if (x >= w)
		x = (x + w) % w;
	if (y >= h)
		y = (y + h) % h;
	return tiles.at(x * h + y);
}

Tile * Tilemap::operator[](size_t x)
{
	return &tiles[x * h];
}
