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

	std::string format(int64_t number) const;
	std::string format(int64_t number, size_t right) const;

private:
	std::vector<Reference<Sprite>> sprites;
	Reference<Sprite> bg, fg;
};

