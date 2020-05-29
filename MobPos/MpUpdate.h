#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

#pragma pack(push, 4)
class MpUpdate
{
public:
	uint64_t tick;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
#pragma pack(pop)
