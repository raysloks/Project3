#include "MpMobHealthUpdate.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpMobHealthUpdate::serialize(std::ostream& os) const
{
	os.write((char*)&this->current, sizeof(this->current));
	os.write((char*)&this->cap, sizeof(this->cap));
	{
		uint16_t size = this->changes.size();
		os.write((char*)&size, sizeof(size));
		os.write((char*)this->changes.data(), sizeof(LinearResourceChange) * size);
	}
}

void MpMobHealthUpdate::deserialize(std::istream& is)
{
	is.read((char*)&this->current, sizeof(this->current));
	is.read((char*)&this->cap, sizeof(this->cap));
	{
		uint16_t size;
		is.read((char*)&size, sizeof(size));
		this->changes.resize(size);
		is.read((char*)this->changes.data(), sizeof(LinearResourceChange) * size);
	}
}

