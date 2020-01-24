#pragma once

#include "CustomBehaviour.h"

#include "Player.h"

class ActionBar :
	public CustomBehaviour
{
public:
	void start();

	void tick(float dt);

	Reference<Player> player;
	std::vector<Reference<Sprite>> sprites, sprites_slot;

	size_t slot_count;
};

