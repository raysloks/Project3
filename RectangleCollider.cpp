#include "RectangleCollider.h"

#include "CircleCollider.h"

void checkCornerCircle(const Vec2& corner, const Vec2& circle_center, const CircleCollider & circle, std::vector<Collision>& collisions)
{
	Vec2 diff = circle_center - corner;

	float distance = diff.Len();
	Vec2 dir = diff / distance;
	if (distance <= circle.r && distance != 0.0f)
	{
		Collision c;
		c.pen = circle.r - distance;
		c.p = dir * (circle.r - c.pen * 0.5f);
		c.n = dir;
		collisions.emplace_back(std::move(c));
	}
}

RectangleCollider::RectangleCollider()
{
}

RectangleCollider::RectangleCollider(const Vec2 & half_size) : half_size(half_size)
{
}

void RectangleCollider::check(const Vec2 & diff, const CircleCollider & circle, std::vector<Collision>& collisions) const
{
	Vec2 sign_diff(copysignf(1.0f, diff.x), copysignf(1.0f, diff.y));
	Vec2 abs_diff(fabsf(diff.x), fabsf(diff.y));

	Vec2 sep = abs_diff - half_size;

	if (sep.x <= circle.r && sep.y <= 0.0f)
	{
		Collision c;
		c.n = Vec2(sign_diff.x, 0.0f);
		c.pen = circle.r - sep.x;
		c.p = diff - c.n * c.pen * 0.5f;
		collisions.emplace_back(std::move(c));
		return;
	}

	if (sep.x <= 0.0f && sep.y <= circle.r)
	{
		Collision c;
		c.n = Vec2(0.0f, sign_diff.y);
		c.pen = circle.r - sep.y;
		c.p = diff - c.n * c.pen * 0.5f;
		collisions.emplace_back(std::move(c));
		return;
	}

	checkCornerCircle(half_size, diff, circle, collisions);
	checkCornerCircle(Vec2(half_size.x, -half_size.y), diff, circle, collisions);
	checkCornerCircle(Vec2(-half_size.x, -half_size.y), diff, circle, collisions);
	checkCornerCircle(Vec2(-half_size.x, half_size.y), diff, circle, collisions);
}
