#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

#include "MpCommand.h"

class MpCancelCommand
{
public:
	MpCommand command;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
