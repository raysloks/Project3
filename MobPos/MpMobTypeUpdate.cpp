#include "MpMobTypeUpdate.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpMobTypeUpdate::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpMobTypeUpdate));
}

void MpMobTypeUpdate::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpMobTypeUpdate));
}

