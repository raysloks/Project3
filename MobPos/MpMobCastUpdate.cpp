#include "MpMobCastUpdate.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpMobCastUpdate::serialize(std::ostream& os) const
{
	{
		uint16_t size = this->queue.size();
		os.write((char*)&size, sizeof(size));
		os.write((char*)this->queue.data(), sizeof(Cast) * size);
	}
}

void MpMobCastUpdate::deserialize(std::istream& is)
{
	{
		uint16_t size;
		is.read((char*)&size, sizeof(size));
		this->queue.resize(size);
		is.read((char*)this->queue.data(), sizeof(Cast) * size);
	}
}

