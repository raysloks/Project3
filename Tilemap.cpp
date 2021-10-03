#include "Tilemap.h"

Tilemap::Tilemap(size_t w, size_t h) : w(w), h(h), tiles(w * h), tile_size(16.0f)
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

const Tile & Tilemap::at(size_t x, size_t y) const
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

uint8_t Tilemap::getEffect(const Vec2 & p) const
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

	if (!tile.effects.dirty)
		updated_tiles.push_back(std::make_pair(x, y));
	tile.effects.dirty = true;
}

void Tilemap::balanceEffect(size_t type, size_t & effect, const Vec2 & p)
{
	auto tile_p = p / tile_size;
	intmax_t x = llroundf(tile_p.x);
	intmax_t y = llroundf(tile_p.y);
	auto& tile = at(x, y);
	tile.effects.balance(type, effect, p - (Vec2(x, y) - Vec2(0.5f, 0.5f)) * tile_size);

	if (!tile.effects.dirty)
		updated_tiles.push_back(std::make_pair(x, y));
	tile.effects.dirty = true;
}

void Tilemap::depositEffect(size_t type, size_t & effect, const Vec2 & p)
{
	auto tile_p = p / tile_size;
	intmax_t x = llroundf(tile_p.x);
	intmax_t y = llroundf(tile_p.y);
	auto& tile = at(x, y);
	tile.effects.deposit(type, effect, p - (Vec2(x, y) - Vec2(0.5f, 0.5f)) * tile_size);

	if (!tile.effects.dirty)
		updated_tiles.push_back(std::make_pair(x, y));
	tile.effects.dirty = true;
}

float Tilemap::getZ(const Vec2& p) const
{
	intmax_t x = floorf(p.x);
	intmax_t y = floorf(p.y);
	auto& tile = at(x, y);
	if (tile.tile == 980)
		return 1.0f + y - p.y;
	if (tile.tile == 990)
		return y - p.y;
	return 0.0f;
}

void Tilemap::refreshUpdatedEffects()
{
	for (auto i : updated_tiles)
	{
		auto x = i.first;
		auto y = i.second;
		auto& tile = at(x, y);
		tile.refreshEffectSprite(Vec2(x, y) * tile_size);
	}

	updated_tiles.clear();
}

size_t Tilemap::getWidth() const
{
	return w;
}

size_t Tilemap::getHeight() const
{
	return h;
}

const Vec2 & Tilemap::getTileSize() const
{
	return tile_size;
}
