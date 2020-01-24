#pragma once

#include <string>

#include "SpriteSheet.h"

class Ability
{
public:
	std::string name;

	float cooldown;

	intmax_t max_charges;

	std::shared_ptr<SpriteSheet> sheet;
};

