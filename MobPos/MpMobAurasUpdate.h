#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <vector>
#include <iostream>

#include "MpAuraUpdate.h"

#pragma pack(push, 1)
class MpMobAurasUpdate
{
public:
	std::vector<MpAuraUpdate> updated;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
#pragma pack(pop)
