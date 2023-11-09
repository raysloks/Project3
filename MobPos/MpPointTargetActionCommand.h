#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

#include "MpActionCommand.h"
#include "Vec3.h"

#pragma pack(push, 1)
class MpPointTargetActionCommand :
	public MpActionCommand
{
public:
	uint8_t getChildTypeIndex() const;
	Vec3 target;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
#pragma pack(pop)
