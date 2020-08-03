#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

#include "MpActionCommand.h"

class MpUnitTargetActionCommand
{
public:
	MpActionCommand action;
	uint64_t target;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
