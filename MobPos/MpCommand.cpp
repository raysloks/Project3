#include "MpCommand.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpCommand::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpCommand));
}

void MpCommand::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpCommand));
}

