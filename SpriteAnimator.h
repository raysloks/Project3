#pragma once

#include "CustomBehaviour.h"

class SpriteAnimator :
	public CustomBehaviour
{
public:
	SpriteAnimator(float speed, float time = 0.0f, size_t frame_count = 0);

	void tick(float dt);

	size_t frame_count;

	float speed;
	float time;

	bool destroy;
};

