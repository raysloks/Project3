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

uint8_t Tilemap::getEffect(const Vec2 & p)
{
	auto tile_p = p / tile_size;
	intmax_t x = llroundf(tile_p.x);
	intmax_t y = llroundf(tile_p.y);
	auto& tile = at(x, y);
	return tile.effects.get(p - (Vec2(x, y) - Vec2(0.5f, 0.5f)) * tile_size);
}

void Tilemap::setEffect(const Vec2 & p, uint8_t effect)
{
	auto tile_p = p / tile_size;
	intmax_t x = llroundf(tile_p.x);
	intmax_t y = llroundf(tile_p.y);
	auto& tile = at(x, y);
	tile.effects.set(p - (Vec2(x, y) - Vec2(0.5f, 0.5f)) * tile_size, effect);

	// TODO only refresh sprite once per frame
	tile.refreshEffectSprite(Vec2(x, y) * tile_size);
}
