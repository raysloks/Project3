#include "MpPlayerInventoryUpdate.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpPlayerInventoryUpdate::serialize(std::ostream& os) const
{
	{
		uint16_t size = this->equipment.size();
		os.write((char*)&size, sizeof(size));
		os.write((char*)this->equipment.data(), sizeof(ItemData) * size);
	}
	{
		uint16_t size = this->inventory.size();
		os.write((char*)&size, sizeof(size));
		os.write((char*)this->inventory.data(), sizeof(ItemData) * size);
	}
}

void MpPlayerInventoryUpdate::deserialize(std::istream& is)
{
	{
		uint16_t size;
		is.read((char*)&size, sizeof(size));
		this->equipment.resize(size);
		is.read((char*)this->equipment.data(), sizeof(ItemData) * size);
	}
	{
		uint16_t size;
		is.read((char*)&size, sizeof(size));
		this->inventory.resize(size);
		is.read((char*)this->inventory.data(), sizeof(ItemData) * size);
	}
}

