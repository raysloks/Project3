#include "MpAttackMoveCommand.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

uint8_t MpAttackMoveCommand::getChildTypeIndex() const { return 2; }
void MpAttackMoveCommand::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpAttackMoveCommand));
}

void MpAttackMoveCommand::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpAttackMoveCommand));
}

