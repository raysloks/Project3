#include "MpAuraUpdate.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpAuraUpdate::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpAuraUpdate));
}

void MpAuraUpdate::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpAuraUpdate));
}

