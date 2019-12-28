#pragma once

#include "System.h"

#include "Sprite.h"

class SpriteRenderSystem :
	public System
{
public:
	SpriteRenderSystem(SDL_Renderer * render);

	void tick(float dt);

	ComponentContainer<Sprite> sprites;

	Vec2 camera_position;

	SDL_Renderer * render;
};

