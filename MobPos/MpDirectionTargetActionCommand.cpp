#include "MpDirectionTargetActionCommand.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpDirectionTargetActionCommand::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpDirectionTargetActionCommand));
}

void MpDirectionTargetActionCommand::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpDirectionTargetActionCommand));
}

