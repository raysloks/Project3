#include "MpMobActionUpdate.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpMobActionUpdate::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpMobActionUpdate));
}

void MpMobActionUpdate::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpMobActionUpdate));
}

