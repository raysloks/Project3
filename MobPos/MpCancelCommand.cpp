#include "MpCancelCommand.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

uint8_t MpCancelCommand::getChildTypeIndex() const { return 3; }
void MpCancelCommand::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpCancelCommand));
}

void MpCancelCommand::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpCancelCommand));
}

