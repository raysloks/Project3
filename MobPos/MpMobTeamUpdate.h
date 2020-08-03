#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

class MpMobTeamUpdate
{
public:
	uint64_t id;
	uint32_t team;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
