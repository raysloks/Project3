#include "MpGuid.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpGuid::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpGuid));
}

void MpGuid::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpGuid));
}

