#pragma once

#include "CustomBehaviour.h"

class FrameRate :
	public CustomBehaviour
{
public:
	void tick(float dt);

private:

	uint64_t start, end, freq;

	std::vector<size_t> sprites;
};

