#include "MpChat.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpChat::serialize(std::ostream& os) const
{
	os.write((char*)&sender, (sizeof(sender) + 3) / 4 * 4);
	{
		uint16_t size = this->message.size();
		os.write((char*)&size, sizeof(size));
		os.write((char*)this->message.data(), size);
	}
}

void MpChat::deserialize(std::istream& is)
{
	is.read((char*)&sender, (sizeof(sender) + 3) / 4 * 4);
	{
		uint16_t size;
		is.read((char*)&size, sizeof(size));
		this->message.resize(size);
		is.read((char*)this->message.data(), size);
	}
}

