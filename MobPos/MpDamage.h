#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

class MpDamage
{
public:
	uint64_t id;
	int64_t damage;
	uint64_t source;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
