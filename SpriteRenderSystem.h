#pragma once

#include "System.h"

#include "Sprite.h"

class SpriteRenderSystem :
	public System
{
public:
	SpriteRenderSystem(ComponentContainer<Sprite> * sprites, SDL_Renderer * render);

	void tick(float dt);

	ComponentContainer<Sprite> sprites;

	SDL_Renderer * render;
};

