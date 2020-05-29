#include "MpUpdate.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpUpdate::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpUpdate));
}

void MpUpdate::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpUpdate));
}

