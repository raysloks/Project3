#pragma once

#include "CustomBehaviour.h"

class FrameRate :
	public CustomBehaviour
{
public:
	void tick(float dt);

private:
	std::vector<double> records;
	std::vector<Reference<Sprite>> sprites;
};

