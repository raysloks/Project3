#pragma once

#include "CustomBehaviour.h"

#include "Item.h"

class Inventory :
	public CustomBehaviour
{
public:
	void start();

	void tick(float dt);

	std::vector<Item> items;

	std::vector<Reference<Sprite>> sprites;

	bool visible;
};

