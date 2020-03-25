#include "HealthDisplay.h"

#include "Player.h"

void HealthDisplay::start()
{
	auto font = SpriteSheet::get("font.png");
	font->rows = 16;
	font->columns = 16;

	for (size_t i = 0; i < 32; ++i)
	{
		auto entity = level->add_entity();
		Entity::adopt(entity, this->entity);
		entity->x = i * 8;

		auto sprite = level->ui_sprites.add(font);
		sprite->color = SDL_Color({ 255, 0, 0, 255 });
		Component::attach(sprite, entity);

		sprites.push_back(sprite);
	}

	player = level->get<Player>();
}

void HealthDisplay::tick(float dt)
{
	entity->x = 16;
	entity->y = srs->getHeight() - 16;

	std::string text = std::to_string(player->hp) + " / " + std::to_string(player->hp_max);
	for (size_t i = 0; i < sprites.size(); ++i)
	{
		auto sprite = sprites[i];
		if (i < text.size())
		{
			size_t c = text[i];
			sprite->subsprite_x = c % 16;
			sprite->subsprite_y = c / 16;
		}
		else
		{
			sprite->subsprite_x = 0;
			sprite->subsprite_y = 0;
		}
	}
}
