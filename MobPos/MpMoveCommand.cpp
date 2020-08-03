#include "MpMoveCommand.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpMoveCommand::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpMoveCommand));
}

void MpMoveCommand::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpMoveCommand));
}

