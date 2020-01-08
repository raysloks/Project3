#include "HealthDisplay.h"

#include "Player.h"

void HealthDisplay::tick(float dt)
{
	if (sprites.empty())
	{
		auto font = SpriteSheet::get("font.png");
		font->rows = 16;
		font->columns = 16;

		for (size_t i = 0; i < 32; ++i)
		{
			Entity entity;
			entity.p.x = 32 + i * 8;
			entity.p.y = 64;

			auto sprite = srs->ui.add(Sprite(font));
			sprite->color = SDL_Color({ 255, 0, 0, 255 });
			entity.addComponent(sprite);

			engine->add_entity(std::move(entity));

			sprites.push_back(sprite);
		}
	}

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
