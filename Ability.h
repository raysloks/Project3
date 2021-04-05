#pragma once

#include <string>
#include <vector>

enum AbilityTargetType
{
	ATT_PASSIVE = 0,
	ATT_SELF = 1,
	ATT_ENEMY_ALIVE = 2,
	ATT_ALLY_ALIVE = 4,
	ATT_ENEMY_DEAD = 8,
	ATT_ALLY_DEAD = 16,
	ATT_POINT = 32
};

class Ability
{
public:
	static void load();
	static const Ability * get(uint64_t id);

	std::string name;

	uint64_t icon;

	uint64_t cooldown, charges;
	uint64_t wind_up, wind_down;

	float range;
};

