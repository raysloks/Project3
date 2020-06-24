#include "UID.h"

#include <sstream>
#include <iomanip>

UID::operator std::string() const
{
	std::stringstream ss;
	ss << std::hex << std::setfill('0') << std::setw(16) << b << std::setw(16) << a;
	return ss.str();
}

bool UID::operator<(const UID& rhs) const
{
	if (b < rhs.b)
		return true;
	return a < rhs.a;
}

bool UID::operator>(const UID& rhs) const
{
	if (b > rhs.b)
		return true;
	return a > rhs.a;
}
