#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

#include "MpMobUpdateData.h"

class MpMobUpdate
{
public:
	uint64_t id;
	MpMobUpdateData data;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
