#include "MpMobSpriteUpdate.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpMobSpriteUpdate::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpMobSpriteUpdate));
}

void MpMobSpriteUpdate::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpMobSpriteUpdate));
}

