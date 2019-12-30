#include "Rectangle.h"

#include "Circle.h"

#include <math.h>

Rectangle::Rectangle(const Vec2 & size) : size(size)
{
}

bool checkCornerCircle(const Vec2& corner, const Vec2 circle_center, const Circle * circle, Collision& c)
{
	Vec2 diff = circle_center - corner;

	float distance = diff.Len();
	Vec2 dir = diff / distance;
	if (distance < circle->r && distance != 0.0f)
	{
		c.pen = circle->r - distance;
		c.p = dir * (circle->r - c.pen * 0.5f);
		c.n = dir;
		return true;
	}
	return false;
}

bool Rectangle::check(const Vec2 & diff, const Shape * other, Collision & c) const
{
	auto circle = dynamic_cast<const Circle*>(other);
	if (circle)
	{
		Vec2 sign_diff(copysignf(1.0f, diff.x), copysignf(1.0f, diff.y));
		Vec2 abs_diff(fabsf(diff.x), fabsf(diff.y));

		Vec2 sep = abs_diff - size;

		if (sep.x < circle->r && sep.y < 0.0f)
		{
			c.n = Vec2(sign_diff.x, 0.0f);
			c.pen = circle->r - sep.x;
			c.p = diff - c.n * c.pen * 0.5f;
			return true;
		}

		if (sep.x < 0.0f && sep.y < circle->r)
		{
			c.n = Vec2(0.0f, sign_diff.y);
			c.pen = circle->r - sep.y;
			c.p = diff - c.n * c.pen * 0.5f;
			return true;
		}

		if (checkCornerCircle(size, diff, circle, c))
			return true;
		if (checkCornerCircle(Vec2(size.x, -size.y), diff, circle, c))
			return true;
		if (checkCornerCircle(Vec2(-size.x, -size.y), diff, circle, c))
			return true;
		if (checkCornerCircle(Vec2(-size.x, size.y), diff, circle, c))
			return true;
	}
	return false;
}
