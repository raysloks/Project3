#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <vector>
#include <iostream>

#include "LinearResourceChange.h"

class MpMobHealthUpdate
{
public:
	uint64_t id;
	int64_t current;
	int64_t cap;
	std::vector<LinearResourceChange> changes;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
