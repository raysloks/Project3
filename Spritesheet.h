#pragma once

#include "Resource.h"

class Spritesheet :
	public Resource<Spritesheet>
{
public:
	Spritesheet(const std::string& fname);

	int counter;
};

