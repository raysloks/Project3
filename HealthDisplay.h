#pragma once

#include "CustomBehaviour.h"

class Player;

class HealthDisplay :
	public CustomBehaviour
{
public:
	void start();

	void tick(float dt);

	Reference<Player> player;

private:
	std::vector<Reference<Sprite>> sprites;
};

