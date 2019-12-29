#include "FrameRate.h"

#include "Spritesheet.h"

void FrameRate::tick(float dt)
{
	if (sprites.empty())
	{
		freq = SDL_GetPerformanceFrequency();

		auto font = Spritesheet::get("font.png");
		font->rows = 16;
		font->columns = 16;

		for (size_t i = 0; i < 4; ++i)
		{
			Entity entity;

			auto sprite = srs->sprites.add(std::move(Sprite(font)));
			sprites.push_back(sprite);
			entity.addComponent(sprite);

			engine->entities.emplace_back(std::move(entity));
		}
	}

	end = SDL_GetPerformanceCounter();
	uint64_t diff = end - start;
	start = end;

	double tps = 1.0 / (double(diff) / freq);

	std::string text = std::to_string(tps);
	for (size_t i = 0; i < sprites.size(); ++i)
	{
		if (i < text.size())
		{
			size_t c = text[i];
			sprites[i]->subsprite_x = c % 16;
			sprites[i]->subsprite_y = c / 16;
		}
		else
		{
			sprites[i]->subsprite_x = 0;
			sprites[i]->subsprite_y = 0;
		}
		sprites[i]->entity->p = srs->camera_position + Vec2(0.0f, -300.0f);
		sprites[i]->entity->p.x += i * 16;
	}
}
