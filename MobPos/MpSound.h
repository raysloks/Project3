#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

#include "Vec3.h"

class MpSound
{
public:
	Vec3 position;
	uint32_t sound_id;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
