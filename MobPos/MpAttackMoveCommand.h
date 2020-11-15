#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

#include "MpCommand.h"
#include "Vec2.h"

#pragma pack(push, 1)
class MpAttackMoveCommand
{
public:
	MpCommand command;
	Vec2 target;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
#pragma pack(pop)
