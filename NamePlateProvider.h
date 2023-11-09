#pragma once

#include "CustomBehaviour.h"

class Mob;
class NamePlate;

class NamePlateProvider :
	public CustomBehaviour
{
public:
	virtual ~NamePlateProvider();

	void start();

	void tick(float dt);

private:
	Reference<Mob> mob;
	std::weak_ptr<NamePlate> namePlate;
};

