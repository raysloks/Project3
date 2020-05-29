#include "MpMobRemoved.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpMobRemoved::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpMobRemoved));
}

void MpMobRemoved::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpMobRemoved));
}

