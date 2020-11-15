#include "FrameRate.h"

void FrameRate::start()
{
	auto font = SpriteSheet::get("font.png");
	font->rows = 16;
	font->columns = 16;

	for (size_t i = 0; i < 32; ++i)
	{
		auto entity = level->add_entity();
		entity->x = 8 + i * 8;
		entity->y = 8;

		auto sprite = level->ui_sprites.add(font);
		Component::attach(sprite, entity);

		sprites.push_back(sprite);
	}

	records.resize(300);
}

void FrameRate::tick(float dt)
{
	double tps = 1.0 / engine->full;

	records[record_counter] = tps;
	++record_counter;
	record_counter %= records.size();

	double sum = 0.0;
	double min = std::numeric_limits<double>::infinity();
	for (auto record : records)
	{
		sum += record;
		if (record < min)
			min = record;
	}

	std::string text = std::to_string(lround(min)) + " " + std::to_string(lround(sum / records.size()));
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
