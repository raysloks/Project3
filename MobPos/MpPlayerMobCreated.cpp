#include "MpPlayerMobCreated.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpPlayerMobCreated::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpPlayerMobCreated));
}

void MpPlayerMobCreated::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpPlayerMobCreated));
}

