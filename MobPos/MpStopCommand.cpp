#include "MpStopCommand.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpStopCommand::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpStopCommand));
}

void MpStopCommand::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpStopCommand));
}

