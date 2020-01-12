#include "TilemapCollider.h"

#include "Tilemap.h"

TilemapCollider::TilemapCollider(Tilemap * tilemap) :
	tilemap(tilemap),
	rectangle(tilemap->tile_size * 0.5f),
	rectangle_w(tilemap->tile_size * Vec2(0.5f, 0.25f)),
	rectangle_h(tilemap->tile_size * Vec2(0.5f, 0.25f))
{
}

void TilemapCollider::checkTile(intmax_t x, intmax_t y, const Vec2 & diff, const CircleCollider & circle, std::vector<Collision>& collisions) const
{
	//if (x >= 0 && x < tilemap->w && y >= 0 && y < tilemap->h && (*tilemap)[x][y].tile & 1)
	if (tilemap->at(x, y).tile & 1)
	{
		//if (tilemap->at(x, y).tile & 32)
		//	rectangle_w.check(diff - Vec2(x, y - 0.25f) * tilemap->tile_size, circle, collisions);
		//else
		rectangle.check(diff - Vec2(x, y) * tilemap->tile_size, circle, collisions);
	}
}

void TilemapCollider::check(const Vec2 & diff, const CircleCollider & circle, std::vector<Collision>& collisions) const
{
	Vec2 scaled_diff = diff / tilemap->tile_size;
	int fx = floorf(scaled_diff.x);
	int fy = floorf(scaled_diff.y);
	int cx = ceilf(scaled_diff.x);
	int cy = ceilf(scaled_diff.y);

	checkTile(fx, fy, diff, circle, collisions);
	checkTile(cx, cy, diff, circle, collisions);
	checkTile(cx, fy, diff, circle, collisions);
	checkTile(fx, cy, diff, circle, collisions);
}
