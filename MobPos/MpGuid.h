#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

class MpGuid
{
public:
	uint64_t low;
	uint64_t high;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};