#pragma once

// WARNING : Auto-generated file, changes made will disappear when re-generated.

#include <iostream>

#pragma pack(push, 1)
class MpAuraUpdate
{
public:
	uint64_t index;
	uint64_t aura_id;
	int64_t start;
	int64_t end;
	int64_t a;
	int64_t b;
	int64_t c;
	int64_t d;

	void serialize(std::ostream& os) const;
	void deserialize(std::istream& is);
};
#pragma pack(pop)
