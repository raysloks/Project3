#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

#pragma pack(push, 1)
class MpGuid
{
public:
	uint64_t low;
	uint64_t high;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
#pragma pack(pop)
