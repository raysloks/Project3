#pragma once

#include "ItemView.h"

#include "SpriteSheet.h"

class ClientItemView :
	public ItemView
{
public:
	ClientItemView(const ItemData& data);

	std::shared_ptr<SpriteSheet> icon;
};

