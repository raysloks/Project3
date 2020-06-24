#pragma once

#include <cstdint>
#include <string>

struct UID
{
	uint64_t a, b;

	operator std::string() const;

	bool operator<(const UID& rhs) const;
	bool operator>(const UID& rhs) const;
};