#include "FrameRate.h"

void FrameRate::start()
{
	auto font = SpriteSheet::get("font.png");
	font->rows = 16;
	font->columns = 16;

	for (size_t i = 0; i < 32; ++i)
	{
		Entity entity;
		entity.p.x = 8 + i * 8;
		entity.p.y = 8;

		auto sprite = srs->ui.add(Sprite(font));
		entity.addComponent(sprite);

		engine->add_entity(std::move(entity));

		sprites.push_back(sprite);
	}
}

void FrameRate::tick(float dt)
{
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

	std::string text = std::to_string(lround(sum / records.size())) + " " + std::to_string(lround(min));
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
