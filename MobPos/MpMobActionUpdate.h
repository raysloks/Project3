#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

#include "Vec2.h"

#pragma pack(push, 1)
class MpMobActionUpdate
{
public:
	uint64_t time;
	uint64_t ability_id;
	Vec2 target_point;
	uint64_t target_id;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
#pragma pack(pop)
