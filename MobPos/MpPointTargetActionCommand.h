#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

#include "MpActionCommand.h"
#include "Vec2.h"

class MpPointTargetActionCommand
{
public:
	MpActionCommand action;
	Vec2 target;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
