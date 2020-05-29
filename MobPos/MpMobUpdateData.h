#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

#include "../Vec3.h"

#pragma pack(push, 4)
class MpMobUpdateData
{
public:
	Vec3 position;
	Vec3 velocity;
	Vec3 facing;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
#pragma pack(pop)
