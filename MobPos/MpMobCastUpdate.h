#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <vector>
#include <iostream>

#include "Cast.h"

#pragma pack(push, 1)
class MpMobCastUpdate
{
public:
	std::vector<Cast> queue;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
#pragma pack(pop)
