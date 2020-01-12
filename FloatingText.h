#pragma once

#include "CustomBehaviour.h"

class FloatingText :
	public CustomBehaviour
{
public:
	FloatingText(const std::string& text, float speed, float duration);

	void start();

	void tick(float dt);

	std::string text;
	float time, speed, duration;
};
