#include "CircleCollider.h"

CircleCollider::CircleCollider()
{
}

CircleCollider::CircleCollider(float r) : r(r)
{
}

void CircleCollider::check(const Vec2 & diff, const CircleCollider & circle, std::vector<Collision>& collisions) const
{
	float distance = diff.Len();
	Vec2 dir = diff / distance;
	float r_sum = r + circle.r;
	if (distance <= r_sum && distance != 0.0f)
	{
		Collision c;
		c.pen = r_sum - distance;
		c.p = dir * (r - c.pen * 0.5f);
		c.n = dir;
		collisions.emplace_back(std::move(c));
	}
}
