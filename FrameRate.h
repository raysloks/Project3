#pragma once

#include "CustomBehaviour.h"

class FrameRate :
	public CustomBehaviour
{
public:
	void tick(float dt);

	uint64_t start, end, freq;

	std::vector<size_t> sprites;
};

