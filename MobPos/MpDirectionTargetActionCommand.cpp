#include "MpDirectionTargetActionCommand.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

#include "MpPointTargetActionCommand.h"
#include "MpUnitTargetActionCommand.h"

void MpDirectionTargetActionCommand::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpDirectionTargetActionCommand));
}

void MpDirectionTargetActionCommand::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpDirectionTargetActionCommand));
}

