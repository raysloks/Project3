#include "MpPlayerMobCreated.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpPlayerMobCreated::serialize(std::ostream& os) const
{
	os.write((char*)&this->id, sizeof(this->id));
	{
		uint16_t size = this->name.size();
		os.write((char*)&size, sizeof(size));
		os.write((char*)this->name.data(), size);
	}
	this->inventory.serialize(os);
	os.write((char*)&this->experience, sizeof(this->experience));
}

void MpPlayerMobCreated::deserialize(std::istream& is)
{
	is.read((char*)&this->id, sizeof(this->id));
	{
		uint16_t size;
		is.read((char*)&size, sizeof(size));
		this->name.resize(size);
		is.read((char*)this->name.data(), size);
	}
	this->inventory.deserialize(is);
	is.read((char*)&this->experience, sizeof(this->experience));
}

