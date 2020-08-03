#include "MpPointTargetActionCommand.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpPointTargetActionCommand::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpPointTargetActionCommand));
}

void MpPointTargetActionCommand::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpPointTargetActionCommand));
}

