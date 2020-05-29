#pragma once

#include "TextDisplay.h"

class Inspector :
	public TextDisplay
{
public:
	void start();

	void tick(float dt);

	size_t index;
};

