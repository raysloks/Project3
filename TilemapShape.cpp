#include "TilemapShape.h"

#include "Rectangle.h"

TilemapShape::TilemapShape(Tilemap * tilemap) : tilemap(tilemap), rectangle(tilemap->tile_size * 0.5f)
{
}

void TilemapShape::checkTile(int x, int y, const Vec2& diff, const Shape * other, std::vector<Collision>& collisions) const
{
	if (x >= 0 && x < tilemap->w && y >= 0 && y < tilemap->h && tilemap->tiles[x + y * tilemap->w])
		rectangle.check(diff - Vec2(x, y) * tilemap->tile_size, other, collisions);
}

void TilemapShape::check(const Vec2& diff, const Shape * other, std::vector<Collision>& collisions) const
{
	Vec2 scaled_diff = diff / tilemap->tile_size;
	int fx = floorf(scaled_diff.x);
	int fy = floorf(scaled_diff.y);
	int cx = ceilf(scaled_diff.x);
	int cy = ceilf(scaled_diff.y);

	checkTile(fx, fy, diff, other, collisions);
	checkTile(cx, cy, diff, other, collisions);
	checkTile(cx, fy, diff, other, collisions);
	checkTile(fx, cy, diff, other, collisions);
}
