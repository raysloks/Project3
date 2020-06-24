#include "MpAttack.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpAttack::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpAttack));
}

void MpAttack::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpAttack));
}

