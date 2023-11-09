#include "MpPlayerMobAbilitiesUpdate.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpPlayerMobAbilitiesUpdate::serialize(std::ostream& os) const
{
	os.write((char*)&this->id, sizeof(this->id));
	{
		uint16_t size = this->abilities.size();
		os.write((char*)&size, sizeof(size));
		os.write((char*)this->abilities.data(), sizeof(uint64_t) * size);
	}
}

void MpPlayerMobAbilitiesUpdate::deserialize(std::istream& is)
{
	is.read((char*)&this->id, sizeof(this->id));
	{
		uint16_t size;
		is.read((char*)&size, sizeof(size));
		this->abilities.resize(size);
		is.read((char*)this->abilities.data(), sizeof(uint64_t) * size);
	}
}

