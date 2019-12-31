#include "Circle.h"

#include "Rectangle.h"

Circle::Circle(float r) : r(r)
{
}

void Circle::check(const Vec2& diff, const Shape * other, std::vector<Collision>& collisions) const
{
	auto circle = dynamic_cast<const Circle*>(other);
	if (circle)
	{
		float distance = diff.Len();
		Vec2 dir = diff / distance;
		float r_sum = r + circle->r;
		if (distance < r_sum && distance != 0.0f)
		{
			Collision c;
			c.pen = r_sum - distance;
			c.p = dir * (r - c.pen * 0.5f);
			c.n = dir;
			collisions.emplace_back(std::move(c));
		}
		return;
	}
	if (other)
	{
		size_t i = collisions.size();
		other->check(-diff, this, collisions);
		for (; i < collisions.size(); ++i)
		{
			collisions[i].flip();
		}
	}
}