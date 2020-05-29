#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

class MpMobRemoved
{
public:
	uint16_t id;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
