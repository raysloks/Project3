#include "Inventory.h"

void Inventory::start()
{
	visible = false;

	input->addKeyDownCallback(SDLK_e, [this]()
		{
			visible = !visible;
		});

	items.resize(64);
}

void Inventory::tick(float dt)
{
	entity->x = 16;
	entity->y = visible ? 16 : 9001;

	for (size_t i = sprites.size(); i < items.size(); ++i)
	{
		auto entity = level->add_entity();
		Entity::adopt(entity, this->entity);
		entity->x = 17 * (i % 8);
		entity->y = 17 * (i / 8);

		auto sprite = level->ui_sprites.add();

		sprite->sort = 512;
		//sprite->scale = 0.5f;

		Component::attach(sprite, entity);

		sprites.push_back(sprite);
	}

	auto cursor = input->getCursor() / srs->getScale();

	for (size_t i = 0; i < sprites.size(); ++i)
	{
		sprites[i]->sheet = SpriteSheet::get("slot.png");
		
		Vec2 p = sprites[i]->entity->getPosition();
		if (cursor.x > p.x - 8 && cursor.x < p.x + 8 && cursor.y > p.y - 8 && cursor.y < p.y + 8)
		{
			sprites[i]->color = SDL_Color({ 255, 255, 255, 255 });
			sprites[i]->sort = 1024;
		}
		else
		{
			sprites[i]->color = SDL_Color({ 200, 200, 200, 255 });
			sprites[i]->sort = 512;
		}
	}
}
