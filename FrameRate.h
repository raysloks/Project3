#pragma once

#include "CustomBehaviour.h"

class FrameRate :
	public CustomBehaviour
{
public:
	void start();

	void tick(float dt);

	size_t max_frame_count;

private:
	std::vector<double> records;
	std::vector<Reference<Sprite>> sprites;
};

