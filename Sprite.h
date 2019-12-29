#pragma once

#include "Component.h"

#include <SDL.h>

#include <memory>

class Spritesheet;

class Sprite :
	public Component
{
public:
	Sprite();
	Sprite(const std::shared_ptr<Spritesheet>& sheet);
	Sprite(const std::string& sheet);

	std::shared_ptr<Spritesheet> sheet;
	size_t subsprite_x, subsprite_y;
};

