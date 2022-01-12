#pragma once

#include "CustomBehaviour.h"

class ModelAnimator :
	public CustomBehaviour
{
public:
	ModelAnimator();
	ModelAnimator(const std::string& action_name, float speed = 1.0f, float time = 0.0f, bool destroy = false);

	void tick(float dt);

	std::string action_name;

	float speed;
	float time;

	bool destroy;
};

