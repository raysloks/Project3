#include "MpCommandQueue.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpCommandQueue::serialize(std::ostream& os) const
{
	os.write((char*)&this->index, sizeof(this->index));
	{
		uint16_t size = this->commands.size();
		os.write((char*)&size, sizeof(size));
		for (size_t i = 0; i < size; ++i)
		{
			auto&& element = this->commands[i];
	element.serialize(os);
		}
	}
}

void MpCommandQueue::deserialize(std::istream& is)
{
	is.read((char*)&this->index, sizeof(this->index));
	{
		uint16_t size;
		is.read((char*)&size, sizeof(size));
		this->commands.resize(size);
		for (size_t i = 0; i < size; ++i)
		{
			auto&& element = this->commands[i];
	element.deserialize(is);
		}
	}
}

