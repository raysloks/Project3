#include "MpSound.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpSound::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpSound));
}

void MpSound::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpSound));
}

