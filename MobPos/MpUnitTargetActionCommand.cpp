#include "MpUnitTargetActionCommand.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

uint8_t MpUnitTargetActionCommand::getChildTypeIndex() const { return 8; }
void MpUnitTargetActionCommand::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpUnitTargetActionCommand));
}

void MpUnitTargetActionCommand::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpUnitTargetActionCommand));
}

