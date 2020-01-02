#include "Sprite.h"

Sprite::Sprite()
{
	this->sheet = nullptr;

	subsprite_x = 0;
	subsprite_y = 0;

	sort = 0.0f;
	
	rotation = 0.0f;

	flip = SDL_FLIP_NONE;
}

Sprite::Sprite(const std::shared_ptr<Spritesheet>& sheet)
{
	this->sheet = sheet;

	subsprite_x = 0;
	subsprite_y = 0;

	sort = 0.0f;

	rotation = 0.0f;

	flip = SDL_FLIP_NONE;
}

Sprite::Sprite(const std::string & sheet)
{
	this->sheet = Spritesheet::get(sheet);

	subsprite_x = 0;
	subsprite_y = 0;

	sort = 0.0f;

	rotation = 0.0f;

	flip = SDL_FLIP_NONE;
}
