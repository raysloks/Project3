#include "MpAttackCommand.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

uint8_t MpAttackCommand::getChildTypeIndex() const { return 1; }
void MpAttackCommand::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpAttackCommand));
}

void MpAttackCommand::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpAttackCommand));
}

