#include "MpHeartbeat.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpHeartbeat::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpHeartbeat));
}

void MpHeartbeat::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpHeartbeat));
}

