#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <vector>
#include <iostream>

#include "Vec2.h"

class MpPath
{
public:
	uint64_t id;
	uint64_t time;
	std::vector<Vec2> points;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
