#include "FrameRate.h"

void FrameRate::tick(float dt)
{
	if (sprites.empty())
	{
		freq = SDL_GetPerformanceFrequency();

		auto font = Spritesheet::get("font.png");
		font->rows = 16;
		font->columns = 16;

		for (size_t i = 0; i < 32; ++i)
		{
			Entity entity;
			entity.p.x = 32 + i * 8;
			entity.p.y = 32;

			auto sprite = srs->ui.add(Sprite(font));
			sprites.push_back(sprite);
			entity.addComponent(sprite);

			engine->add_entity(std::move(entity));
		}
	}

	end = SDL_GetPerformanceCounter();
	uint64_t diff = end - start;
	start = end;

	double tps = 1.0 / (double(diff) / freq);

	std::string text = std::to_string(lround(tps));
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
	}
}
