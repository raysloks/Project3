#pragma once

#include <vector>

#include "Vec2.h"

class MobPath
{
public:

	void clear();

	bool finished() const;

	Vec2 getPosition() const;
	void move(float distance);

	Vec2 getDirection() const;

	std::vector<Vec2> points;
	std::vector<float> distances;
	size_t index;
	float distance;
};

