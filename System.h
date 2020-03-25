#pragma once

#include "Entity.h"
#include "ComponentContainer.h"

class Engine;
class Level;

class System
{
public:
	System() {}
	virtual ~System() {}

	virtual void tick(float dt) {}

	Engine * engine;
	Level * level;
};

