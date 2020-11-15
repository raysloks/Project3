#include "MpMobHealthUpdate.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpMobHealthUpdate::serialize(std::ostream& os) const
{
	os.write((char*)&current, (sizeof(current) + 3) / 4 * 4);
	os.write((char*)&cap, (sizeof(cap) + 3) / 4 * 4);
	{
		uint16_t size = this->changes.size();
		os.write((char*)&size, sizeof(size));
		os.write((char*)this->changes.data(), sizeof(LinearResourceChange) * size);
	}
}

void MpMobHealthUpdate::deserialize(std::istream& is)
{
	is.read((char*)&current, (sizeof(current) + 3) / 4 * 4);
	is.read((char*)&cap, (sizeof(cap) + 3) / 4 * 4);
	{
		uint16_t size;
		is.read((char*)&size, sizeof(size));
		this->changes.resize(size);
		is.read((char*)this->changes.data(), sizeof(LinearResourceChange) * size);
	}
}

