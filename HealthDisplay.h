#pragma once

#include "CustomBehaviour.h"

class Mob;

class HealthDisplay :
	public CustomBehaviour
{
public:
	void start();

	void tick(float dt);

	Reference<Mob> player;

	std::string format(int64_t number) const;
	std::string format(int64_t number, size_t right) const;

private:
	std::vector<Reference<Sprite>> sprites;
	Reference<Sprite> bg, fg;
};

