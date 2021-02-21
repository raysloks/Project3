#include "MpPath.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpPath::serialize(std::ostream& os) const
{
	os.write((char*)&this->time, (sizeof(this->time) + 3) / 4 * 4);
	{
		uint16_t size = this->points.size();
		os.write((char*)&size, sizeof(size));
		os.write((char*)this->points.data(), sizeof(Vec2) * size);
	}
}

void MpPath::deserialize(std::istream& is)
{
	is.read((char*)&this->time, (sizeof(this->time) + 3) / 4 * 4);
	{
		uint16_t size;
		is.read((char*)&size, sizeof(size));
		this->points.resize(size);
		is.read((char*)this->points.data(), sizeof(Vec2) * size);
	}
}

