#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <string>
#include <iostream>

#pragma pack(push, 1)
class MpChat
{
public:
	uint64_t sender;
	std::string message;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
#pragma pack(pop)
