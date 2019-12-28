#pragma once

#include "Component.h"

#include <SDL.h>

class Sprite :
	public Component
{
public:
	SDL_Texture * texture;
	SDL_Rect rect;
};

