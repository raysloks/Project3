#include "MpAuthentication.h"

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

void MpAuthentication::serialize(std::ostream& os) const
{
	os.write((char*)&this->character_id, sizeof(this->character_id));
	{
		uint16_t size = this->auth_token.size();
		os.write((char*)&size, sizeof(size));
		os.write((char*)this->auth_token.data(), sizeof(uint8_t) * size);
	}
}

void MpAuthentication::deserialize(std::istream& is)
{
	is.read((char*)&this->character_id, sizeof(this->character_id));
	{
		uint16_t size;
		is.read((char*)&size, sizeof(size));
		this->auth_token.resize(size);
		is.read((char*)this->auth_token.data(), sizeof(uint8_t) * size);
	}
}

