#pragma once

#include <vector>

#include "Vec2.h"

#include "Tile.h"

#include <set>

class Tilemap
{
public:
	Tilemap(size_t w, size_t h);

	Tile & at(size_t x, size_t y);
	const Tile & at(size_t x, size_t y) const;
	Tile * operator[](size_t x);

	uint8_t getEffect(const Vec2& p) const;
	void setEffect(const Vec2& p, uint8_t effect);

	float getZ(const Vec2& p) const;

	void refreshUpdatedEffects();

	size_t getWidth() const;
	size_t getHeight() const;

	const Vec2 & getTileSize() const;

private:

	std::vector<Tile> tiles;
	size_t w, h;

	Vec2 tile_size;

	std::vector<std::pair<size_t, size_t>> updated_tiles;
};

