#pragma once

#include "Component.h"

#include <SDL.h>

class Sprite :
	public Component
{
public:
	SDL_Surface * surface;
	SDL_Rect rect;
};

