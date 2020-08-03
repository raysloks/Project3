#include "MpMobStateUpdate.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpMobStateUpdate::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpMobStateUpdate));
}

void MpMobStateUpdate::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpMobStateUpdate));
}

