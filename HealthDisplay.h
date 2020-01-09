#pragma once

#include "CustomBehaviour.h"

class Player;

class HealthDisplay :
	public CustomBehaviour
{
public:
	void start();

	void tick(float dt);

	std::shared_ptr<Player> player;

private:
	std::vector<Reference<Sprite>> sprites;
};

