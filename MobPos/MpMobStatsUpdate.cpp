#include "MpMobStatsUpdate.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpMobStatsUpdate::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpMobStatsUpdate));
}

void MpMobStatsUpdate::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpMobStatsUpdate));
}

