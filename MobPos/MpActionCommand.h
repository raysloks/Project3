#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

#include "MpCommand.h"

#pragma pack(push, 1)
class MpActionCommand
{
public:
	MpCommand command;
	uint8_t action;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
#pragma pack(pop)
