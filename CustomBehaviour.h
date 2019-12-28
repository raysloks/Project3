#pragma once

#include "Component.h"

class CustomBehaviour :
	public Component
{
public:
	virtual void tick(float dt);
};

