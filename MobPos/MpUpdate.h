#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

class MpUpdate
{
public:
	uint32_t tick;
	uint64_t time;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
