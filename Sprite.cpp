#include "Sprite.h"

#include "Spritesheet.h"

Sprite::Sprite()
{
	this->sheet = nullptr;
}

Sprite::Sprite(const std::shared_ptr<Spritesheet>& sheet)
{
	this->sheet = sheet;
}

Sprite::Sprite(const std::string & sheet)
{
	this->sheet = Spritesheet::get(sheet);
}
