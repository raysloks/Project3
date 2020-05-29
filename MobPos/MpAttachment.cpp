#include "MpAttachment.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpAttachment::serialize(std::ostream& os) const
{
	os.write((char*)this, sizeof(MpAttachment));
}

void MpAttachment::deserialize(std::istream& is)
{
	is.read((char*)this, sizeof(MpAttachment));
}

