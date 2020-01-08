#pragma once

#include "System.h"

#include <memory>

class CustomBehaviour;

class CustomBehaviourSystem :
	public System
{
public:
	void tick(float dt);

	CustomBehaviour * add(const std::shared_ptr<CustomBehaviour> & behaviour);

private:
	std::vector<std::shared_ptr<CustomBehaviour>> added;
	ComponentContainer<std::shared_ptr<CustomBehaviour>> behaviours;
};

