#include "MpMobAurasUpdate.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpMobAurasUpdate::serialize(std::ostream& os) const
{
	{
		uint16_t size = this->updated.size();
		os.write((char*)&size, sizeof(size));
		os.write((char*)this->updated.data(), sizeof(MpAuraUpdate) * size);
	}
}

void MpMobAurasUpdate::deserialize(std::istream& is)
{
	{
		uint16_t size;
		is.read((char*)&size, sizeof(size));
		this->updated.resize(size);
		is.read((char*)this->updated.data(), sizeof(MpAuraUpdate) * size);
	}
}

