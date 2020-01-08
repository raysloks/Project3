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
				pixel = SDL_Color({ 80, 0, 0, uint8_t(256 - 32 / effect) });
		}
	}

	return sheet;
}

// SUPER HACKY fix quickly TODO
#include "CustomBehaviour.h"

void Tile::refreshEffectSprite(const Vec2 & p)
{
	if (effect_sprite)
	{
		effect_sprite->sheet = makeEffectSheet()->makeIsometricFloorLossless();
	}
	else
	{
		Entity entity;
		entity.p = p;

		Sprite sprite(makeEffectSheet()->makeIsometricFloorLossless());
		sprite.sort = -128;

		effect_sprite = CustomBehaviour::engine->srs->sprites.add(std::move(sprite));

		entity.addComponent(effect_sprite);

		CustomBehaviour::engine->add_entity(std::move(entity));
	}
}
