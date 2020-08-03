#include "MpMobTeamUpdate.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpMobTeamUpdate::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpMobTeamUpdate));
}

void MpMobTeamUpdate::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpMobTeamUpdate));
}

