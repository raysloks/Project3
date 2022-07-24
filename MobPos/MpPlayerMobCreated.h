#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <string>
#include <iostream>

#include "MpPlayerExperienceUpdate.h"
#include "MpPlayerInventoryUpdate.h"

#pragma pack(push, 1)
class MpPlayerMobCreated
{
public:
	uint64_t id;
	std::string name;
	MpPlayerInventoryUpdate inventory;
	MpPlayerExperienceUpdate experience;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
#pragma pack(pop)
