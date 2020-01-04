#pragma once

#include "Component.h"

#include <SDL.h>

#include <memory>

#include "SpriteSheet.h"

class Sprite :
	public Component
{
public:
	Sprite();
	Sprite(const std::shared_ptr<SpriteSheet>& sheet);
	Sprite(const std::string& sheet);

	std::shared_ptr<SpriteSheet> sheet;
	size_t subsprite_x, subsprite_y;
	float sort;

	float rotation;

	SDL_RendererFlip flip;

	SDL_Color color;
};

