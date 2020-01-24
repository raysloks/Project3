#pragma once

#include <memory>

#include "Ability.h"

class AbilitySlot
{
public:
	std::shared_ptr<const Ability> ability;

	float charges;
};

