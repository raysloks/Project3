#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

#include "Vec3.h"

class MpMobUpdateData
{
public:
	uint64_t tick;
	Vec3 position;
	Vec3 velocity;
	Vec3 facing;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
