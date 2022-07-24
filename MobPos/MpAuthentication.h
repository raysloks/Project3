#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <boost/uuid/uuid.hpp>
#include <vector>
#include <iostream>

#pragma pack(push, 1)
class MpAuthentication
{
public:
	boost::uuids::uuid character_id;
	std::vector<uint8_t> auth_token;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
#pragma pack(pop)
