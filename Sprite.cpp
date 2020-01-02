#include "Sprite.h"

Sprite::Sprite()
{
	this->sheet = nullptr;

	subsprite_x = 0;
	subsprite_y = 0;

	sort = 0.0f;
	
	rotation = 0.0f;

	flip = SDL_FLIP_NONE;
	
	color.r = 255;
	color.g = 255;
	color.b = 255;
	color.a = 255;
}

Sprite::Sprite(const std::shared_ptr<Spritesheet>& sheet) : Sprite()
{
	this->sheet = sheet;
}

Sprite::Sprite(const std::string & sheet) : Sprite()
{
	this->sheet = Spritesheet::get(sheet);
}