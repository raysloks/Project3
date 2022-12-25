#include "DynamicState.h"

bool DynamicState::operator==(const DynamicState& rhs) const
{
	return !(*this != rhs);
}

bool DynamicState::operator!=(const DynamicState& rhs) const
{
	if (scissor.offset.x != rhs.scissor.offset.x)
		return true;
	if (scissor.offset.y != rhs.scissor.offset.y)
		return true;
	if (scissor.extent.width != rhs.scissor.extent.width)
		return true;
	if (scissor.extent.height != rhs.scissor.extent.height)
		return true;
	return false;
}
