#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

#include "MobStatBlock.h"

#pragma pack(push, 1)
class MpMobStatsUpdate
{
public:
	uint64_t id;
	MobStatBlock stats;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
#pragma pack(pop)
