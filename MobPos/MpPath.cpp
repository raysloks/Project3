#include "MpPath.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpPath::serialize(std::ostream& os) const
{
	os.write((char*)&this->time, sizeof(this->time));
	{
		uint16_t size = this->points.size();
		os.write((char*)&size, sizeof(size));
		os.write((char*)this->points.data(), sizeof(Vec3) * size);
	}
	{
		uint16_t size = this->distances.size();
		os.write((char*)&size, sizeof(size));
		os.write((char*)this->distances.data(), sizeof(float) * size);
	}
}

void MpPath::deserialize(std::istream& is)
{
	is.read((char*)&this->time, sizeof(this->time));
	{
		uint16_t size;
		is.read((char*)&size, sizeof(size));
		this->points.resize(size);
		is.read((char*)this->points.data(), sizeof(Vec3) * size);
	}
	{
		uint16_t size;
		is.read((char*)&size, sizeof(size));
		this->distances.resize(size);
		is.read((char*)this->distances.data(), sizeof(float) * size);
	}
}

