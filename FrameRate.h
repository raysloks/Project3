#pragma once

#include "CustomBehaviour.h"

class FrameRate :
	public CustomBehaviour
{
public:
	void start();

	void tick(float dt);

private:
	std::vector<double> records;
	size_t record_counter;
	std::vector<Reference<Sprite>> sprites;
};

