#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <vector>
#include <iostream>

#pragma pack(push, 1)
class MpPlayerMobAbilitiesUpdate
{
public:
	uint64_t id;
	std::vector<uint64_t> abilities;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
#pragma pack(pop)
