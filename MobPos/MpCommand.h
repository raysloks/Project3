#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

class MpCommand
{
public:
	uint64_t time;
	uint8_t index;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
