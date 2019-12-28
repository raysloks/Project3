#pragma once

#include "System.h"

#include <memory>

class CustomBehaviour;

class CustomBehaviourSystem :
	public System
{
public:
	void tick(float dt);

	ComponentContainer<std::shared_ptr<CustomBehaviour>> behaviours;
};

