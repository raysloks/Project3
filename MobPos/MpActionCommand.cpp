#include "MpActionCommand.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

MpActionCommand::~MpActionCommand() {}
uint8_t MpActionCommand::getChildTypeIndex() const { return 0; }
void MpActionCommand::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpActionCommand));
}

void MpActionCommand::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpActionCommand));
}

