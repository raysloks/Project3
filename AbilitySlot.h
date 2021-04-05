#pragma once

#include <memory>

#include "Ability.h"

class AbilitySlot
{
public:
	const Ability * ability;

	float charges;
};

