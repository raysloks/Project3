#include "Ability.h"

#include <fstream>
#include <iostream>

#include "Coal.h"

std::vector<Ability> abilities;

void Ability::load()
{
	std::ifstream f("../../../abilities.txt", std::ifstream::in);
	
	auto coal = Coal::parse(f);

	if (coal)
	{
		coal->print(std::cout);
		for (auto element : coal->elements)
		{
			Ability ability;
			ability.name = element["name"].string;
			ability.wind_up = element["wind_up"];
			ability.wind_down = element["wind_down"];
			ability.range = element["range"];
			ability.cooldown = element["cooldown"];

			abilities.push_back(ability);
		}
	}

	std::cout << "Loaded " << abilities.size() << " abilities." << std::endl;
}

const Ability * Ability::get(uint64_t id)
{
	if (id >= abilities.size())
		return nullptr;
	return &abilities[id];
}
