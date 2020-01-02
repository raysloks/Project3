#pragma once

#include "CustomBehaviour.h"

#include <memory>
#include <functional>

class Collision;

class Enemy :
	public CustomBehaviour
{
public:
	void tick(float dt);
};

