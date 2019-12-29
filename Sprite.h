#pragma once

#include "Component.h"

#include <SDL.h>

#include <memory>

class Spritesheet;

class Sprite :
	public Component
{
public:
	std::shared_ptr<Spritesheet> sheet;
	SDL_Rect rect;
};

