#pragma once

#include "System.h"

#include "Sprite.h"

class SpriteRender :
	public System
{
public:
	SpriteRender(ComponentContainer<Sprite> * sprites, SDL_Surface * surface);

	void tick(float dt);

	ComponentContainer<Sprite> sprites;

	SDL_Surface * surface;
};

