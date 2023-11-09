#include "MpPointTargetActionCommand.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

uint8_t MpPointTargetActionCommand::getChildTypeIndex() const { return 6; }
void MpPointTargetActionCommand::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpPointTargetActionCommand));
}

void MpPointTargetActionCommand::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpPointTargetActionCommand));
}

