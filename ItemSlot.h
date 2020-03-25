#pragma once

#include <memory>

#include "Item.h"

class ItemSlot
{
public:
	bool allows(const std::shared_ptr<Item>& item) const;

	bool exchange(ItemSlot& other);

	std::shared_ptr<Item> item;

	uint64_t required_flags;
};

