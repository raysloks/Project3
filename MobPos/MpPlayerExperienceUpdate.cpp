#include "MpPlayerExperienceUpdate.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpPlayerExperienceUpdate::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpPlayerExperienceUpdate));
}

void MpPlayerExperienceUpdate::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpPlayerExperienceUpdate));
}

