#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

#include "Vec3.h"

#pragma pack(push, 1)
class MpCommand
{
public:
	virtual ~MpCommand();
	virtual uint8_t getChildTypeIndex() const;
	uint64_t time;
	Vec3 position;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
#pragma pack(pop)
