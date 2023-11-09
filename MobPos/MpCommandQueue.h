#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <vector>
#include <iostream>

#include "MpCommandWrapper.h"

#pragma pack(push, 1)
class MpCommandQueue
{
public:
	int16_t index;
	std::vector<MpCommandWrapper> commands;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
#pragma pack(pop)
