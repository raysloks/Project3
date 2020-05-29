#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

#include "MpGuid.h"
#include "../Vec3.h"

class MpAttachment
{
public:
	MpGuid guid;
	Vec3 offset;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
