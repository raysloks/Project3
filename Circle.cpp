#include "Circle.h"

#include "Rectangle.h"

Circle::Circle(float r) : r(r)
{
}

bool Circle::check(const Vec2& diff, const Shape * other, Collision & c) const
{
	auto circle = dynamic_cast<const Circle*>(other);
	if (circle)
	{
		float distance = diff.Len();
		Vec2 dir = diff / distance;
		float r_sum = r + circle->r;
		if (distance < r_sum && distance != 0.0f)
		{
			c.pen = r_sum - distance;
			c.p = dir * (r - c.pen * 0.5f);
			c.n = dir;
			return true;
		}
	}
	auto rectangle = dynamic_cast<const Rectangle*>(other);
	if (rectangle)
	{
		if (rectangle->check(-diff, this, c))
		{
			c.flip();
			return true;
		}
	}
	return false;
}
