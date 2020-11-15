#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <vector>
#include <iostream>

#include "Vec2.h"

#pragma pack(push, 1)
class MpPath
{
public:
	uint64_t time;
	std::vector<Vec2> points;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
#pragma pack(pop)
