#pragma once

#include "CustomBehaviour.h"

#include "Item.h"

class Inventory :
	public CustomBehaviour
{
public:
	std::vector<Item> item;
};

