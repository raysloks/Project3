#include "MpLinearResourceChange.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpLinearResourceChange::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpLinearResourceChange));
}

void MpLinearResourceChange::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpLinearResourceChange));
}

