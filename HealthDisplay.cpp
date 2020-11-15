#include "HealthDisplay.h"

#include <chrono>

#include "Player.h"

#include "MobPosHandler.h"

void HealthDisplay::start()
{
	auto font = SpriteSheet::get("font.png")->makeOutline({ 0, 0, 0, 255 }, { 255, 255, 255, 255 });;
	font->rows = 16;
	font->columns = 16;

	for (size_t i = 0; i < 32; ++i)
	{
		auto entity = level->add_entity();
		Entity::adopt(entity, this->entity);
		entity->x = i * 8 + 16;

		auto sprite = level->ui_sprites.add(font);
		sprite->color = SDL_Color({ 255, 255, 255, 255 });
		Component::attach(sprite, entity);

		sprites.push_back(sprite);
	}

	{
		auto entity = level->add_entity();
		Entity::adopt(entity, this->entity);
		entity->x = 64;

		bg = level->ui_sprites.add("pixel.png");
		bg->sort = -2.0f;
		bg->scale = Vec2(128 + 2, 16 + 2);
		bg->color = SDL_Color({ 0, 0, 0, 255 });
		Component::attach(bg, entity);
	}

	{
		auto entity = level->add_entity();
		Entity::adopt(entity, this->entity);
		entity->x = 64;

		fg = level->ui_sprites.add("pixel.png");
		fg->sort = -1.0f;
		fg->scale = Vec2(128, 16);
		fg->color = SDL_Color({ 255, 0, 0, 255 });
		Component::attach(fg, entity);
	}
}

void HealthDisplay::tick(float dt)
{
	if (player == nullptr)
		return;

	entity->x = srs->getWidth() / 2 - 64;
	entity->y = srs->getHeight() - 32;

	int64_t current_hp = player->hp.evaluate(net->time);

	fg->scale.x = 128 * current_hp / player->hp.cap;
	fg->entity->x = fg->scale.x / 2;

	std::string text = format(current_hp, 5) + " / " + format(player->hp.cap);
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

std::string HealthDisplay::format(int64_t number) const
{
	if (number < 10000)
		return std::to_string(number);
	return std::to_string(number / 1000) + "k";
}

std::string HealthDisplay::format(int64_t number, size_t right) const
{
	std::string text = format(number);
	while (text.size() < right)
		text = " " + text;
	return text;
}
