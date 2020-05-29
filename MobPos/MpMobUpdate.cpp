#include "MpMobUpdate.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpMobUpdate::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpMobUpdate));
}

void MpMobUpdate::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpMobUpdate));
}

