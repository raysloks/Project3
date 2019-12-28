#pragma once

#include "CustomBehaviour.h"

class Player :
	public CustomBehaviour
{
public:
	void tick(float dt);
};

