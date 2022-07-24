#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <vector>
#include <iostream>

#include "ItemData.h"

#pragma pack(push, 1)
class MpPlayerInventoryUpdate
{
public:
	std::vector<ItemData> equipment;
	std::vector<ItemData> inventory;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
#pragma pack(pop)
