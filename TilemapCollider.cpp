#include "TilemapCollider.h"

#include "Tilemap.h"

TilemapCollider::TilemapCollider(Tilemap * tilemap) :
	tilemap(tilemap),
	rectangle(tilemap->getTileSize() * 0.5f),
	rectangle_w(tilemap->getTileSize() * Vec2(0.5f, 0.0f)),
	rectangle_h(tilemap->getTileSize() * Vec2(0.0f, 0.5f))
{
}

void TilemapCollider::checkTile(intmax_t x, intmax_t y, const Vec2 & diff, const CircleCollider & circle, std::vector<Collision>& collisions) const
{
	//if (x >= 0 && x < tilemap->w && y >= 0 && y < tilemap->h && (*tilemap)[x][y].tile & 1)
	auto tile = tilemap->at(x, y).tile;
	if (tile > 900)
	{
		rectangle_h.check(diff - Vec2(x - 0.5f, y) * tilemap->getTileSize(), circle, collisions);
		rectangle_h.check(diff - Vec2(x + 0.5f, y) * tilemap->getTileSize(), circle, collisions);
		rectangle_w.check(diff - Vec2(x, y + (tile & 2 ? 0.5f : -0.5f)) * tilemap->getTileSize(), circle, collisions);
		return;
	}
	if (tile & 1)
	{
		//if (tilemap->at(x, y).tile & 32)
		//	rectangle_w.check(diff - Vec2(x, y - 0.25f) * tilemap->tile_size, circle, collisions);
		//else
		rectangle.check(diff - Vec2(x, y) * tilemap->getTileSize(), circle, collisions);
	}
}

void TilemapCollider::check(const Vec2 & diff, const CircleCollider & circle, std::vector<Collision>& collisions) const
{
	Vec2 scaled_diff = diff / tilemap->getTileSize();
	int fx = floorf(scaled_diff.x);
	int fy = floorf(scaled_diff.y);
	int cx = ceilf(scaled_diff.x);
	int cy = ceilf(scaled_diff.y);

	checkTile(fx, fy, diff, circle, collisions);
	checkTile(cx, cy, diff, circle, collisions);
	checkTile(cx, fy, diff, circle, collisions);
	checkTile(fx, cy, diff, circle, collisions);
}
