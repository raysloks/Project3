#include "FrameRate.h"

void FrameRate::tick(float dt)
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
			entity.p.y = 32;

			auto sprite = srs->ui.add(Sprite(font));
			entity.addComponent(sprite);

			sprites.push_back(engine->add_entity(std::move(entity)));
		}
	}

	double tps = 1.0 / engine->full;

	records.push_back(tps);
	if (records.size() > 60)
		records.erase(records.begin());

	double sum = 0.0;
	double min = records.front();
	for (auto record : records)
	{
		sum += record;
		if (record < min)
			min = record;
	}

	std::string text = std::to_string(lround(sum / records.size())) + " " + std::to_string(lround(min)) + " " + std::to_string(lround(tps));
	for (size_t i = 0; i < sprites.size(); ++i)
	{
		auto sprite = engine->get_entity(sprites[i])->getComponent<Sprite>();
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
