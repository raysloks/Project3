#include "MpDamage.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpDamage::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpDamage));
}

void MpDamage::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpDamage));
}

