#include "MpCommandQueueAcknowledge.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpCommandQueueAcknowledge::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpCommandQueueAcknowledge));
}

void MpCommandQueueAcknowledge::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpCommandQueueAcknowledge));
}

