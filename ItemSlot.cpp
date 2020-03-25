#include "ItemSlot.h"

bool ItemSlot::allows(const std::shared_ptr<Item>& item) const
{
	if (item)
		return (item->flags & required_flags) == required_flags;
	return true;
}

bool ItemSlot::exchange(ItemSlot & other)
{
	if (allows(other.item) && other.allows(item))
	{
		std::swap(item, other.item);
		return true;
	}
	return false;
}
