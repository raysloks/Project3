#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

#pragma pack(push, 1)
class MpLinearResourceChange
{
public:
	int64_t start;
	int64_t duration;
	int64_t amount;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
#pragma pack(pop)
