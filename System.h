#pragma once

#include "Entity.h"
#include "ComponentContainer.h"

class System
{
public:
	System();
	virtual ~System();

	virtual void tick(float dt);
};

