#include "MpMobUpdateData.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpMobUpdateData::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpMobUpdateData));
}

void MpMobUpdateData::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpMobUpdateData));
}

