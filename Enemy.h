#pragma once

#include "CustomBehaviour.h"

#include <memory>
#include <functional>

class Collision;

class Enemy :
	public CustomBehaviour
{
public:
	~Enemy();

	void tick(float dt);

private:
	std::shared_ptr<std::function<void(const Collision&)>> on_collision;
};

