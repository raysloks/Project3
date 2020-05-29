#include "TextDisplay.h"

#include <cmath>

void TextDisplay::start()
{
	updateSprites();
}

void TextDisplay::setText(const std::string & text)
{
	if (this->text != text)
	{
		this->text = text;
		updateSprites();
	}
}

void TextDisplay::updateSprites()
{
	Vec2 spacing(8.0f, 8.0f);
	float tab = 32.0f;

	size_t i = 0;
	size_t j = 0;
	Vec2 position;
	while (j < text.size())
	{
		auto sprite = getSprite(i);
		sprite->entity->xy = position;

		size_t c = text[j];
		++j;
		switch (c)
		{
		case '\n':
			position.x = 0.0f;
			position.y += spacing.y;
			continue;
		case ' ':
			position.x += spacing.x;
			continue;
		case '	':
			position.x = tab * (std::floorf(position.x / tab) + 1.0f);
			continue;
		default:
			position.x += spacing.x;
		}
		sprite->subsprite_x = c % 16;
		sprite->subsprite_y = c / 16;
		++i;
	}
	while (i < sprites.size())
	{
		auto sprite = getSprite(i);
		sprite->subsprite_x = 0;
		sprite->subsprite_y = 0;
		++i;
	}
}

Reference<Sprite> TextDisplay::getSprite(size_t index)
{
	if (index >= sprites.size())
		sprites.resize(index + 1);
	if (sprites[index] == nullptr)
	{
		auto entity = level->add_entity();
		Entity::adopt(entity, this->entity);

		auto sprite = level->ui_sprites.add(font);
		sprite->color = SDL_Color({ 255, 255, 255, 255 });
		Component::attach(sprite, entity);

		sprites[index] = sprite;
	}
	return sprites[index];
}
