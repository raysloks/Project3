#include "Tile.h"

#include "SpriteSheet.h"

#include <SDL.h>

std::shared_ptr<SpriteSheet> Tile::makeEffectSheet()
{
	auto sheet = std::make_shared<SpriteSheet>(16, 16);

	for (size_t x = 0; x < 16; ++x)
	{
		for (size_t y = 0; y < 16; ++y)
		{
			SDL_Color & pixel = *(SDL_Color*)((uint8_t*)sheet->surface->pixels + y * sheet->surface->pitch + x * 4);
			pixel = SDL_Color({ 0, 0, 0, 0 });
			auto effect = effects.get(x, y);
			if (effect)
				pixel = SDL_Color({ 80, 0, 0, uint8_t(255 - 32 / effect) });
		}
	}

	return sheet;
}

// SUPER HACKY fix quickly TODO
#include "CustomBehaviour.h"

void Tile::refreshEffectSprite(const Vec2 & p)
{

	if (!effect_sprite)
	{
		auto entity = CustomBehaviour::level->add_entity();
		entity->xy = p;

		effect_sprite = CustomBehaviour::level->sprites.add();

		Component::attach(effect_sprite, entity);
	}

	auto sheet = makeEffectSheet();
	if (tile > 900)
	{
		effect_sprite->sheet = sheet->makeMapped(SpriteSheet::get("stairs_effect_map_two.png"));
		effect_sprite->sort = 1;
		effect_sprite->entity->z = tile & 2 ? -16 : 0;
	}
	else
	{
		effect_sprite->sheet = sheet->makeIsometricFloorLossless();
		effect_sprite->sort = -15;
		effect_sprite->entity->z = 0;
	}
}
