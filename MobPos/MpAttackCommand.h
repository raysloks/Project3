#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

#include "MpCommand.h"

#pragma pack(push, 1)
class MpAttackCommand :
	public MpCommand
{
public:
	uint8_t getChildTypeIndex() const;
	uint64_t target;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
#pragma pack(pop)
