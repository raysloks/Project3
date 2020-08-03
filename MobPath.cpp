#include "MobPath.h"

void MobPath::clear()
{
	points.clear();
	distances.clear();
	index = 0;
	distance = 0;
}

bool MobPath::finished() const
{
	return index >= distances.size();
}

Vec2 MobPath::getPosition() const
{
	if (index >= distances.size())
		if (points.empty())
			return Vec2();
		else
			return points.back();
	return points[index] + (points[index + 1] - points[index]) * distance / distances[index];
}

void MobPath::move(float delta)
{
	distance += delta;
	if (delta < 0.0f)
	{
		while (distance < 0.0f)
		{
			if (index == 0)
			{
				distance = 0.0f;
				return;
			}
			--index;
			distance += distances[index];
		}
	}
	else
	{
		if (index >= distances.size())
			return;
		while (distance > distances[index])
		{
			distance -= distances[index];
			++index;
			if (index >= distances.size())
				return;
		}
	}
}

Vec2 MobPath::getDirection() const
{
	if (index >= distances.size())
		return Vec2();
	return (points[index + 1] - points[index]).Normalized();
}
