#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

class MpMobSpriteUpdate
{
public:
	uint64_t id;
	uint64_t sprite;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
