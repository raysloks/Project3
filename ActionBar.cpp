#include "ActionBar.h"

void ActionBar::start()
{
	slot_count = 8;

	player = level->get<Player>();
}

void ActionBar::tick(float dt)
{
	entity->x = srs->getWidth() / 2;
	entity->y = srs->getHeight() - 12;

	for (size_t i = sprites_slot.size(); i < slot_count; ++i)
	{
		auto entity = level->add_entity();
		Entity::adopt(entity, this->entity);
		entity->x = 17 * (i - slot_count * 0.5f + 0.5f);

		auto sprite = level->ui_sprites.add("slot.png");

		sprite->sort = 512;
		//sprite->scale = 0.5f;

		Component::attach(sprite, entity);

		sprites_slot.push_back(sprite);
	}

	for (size_t i = sprites.size(); i < player->abilities.size(); ++i)
	{
		auto entity = level->add_entity();
		Entity::adopt(entity, this->entity);
		entity->x = 17 * (i - slot_count * 0.5f + 0.5f);

		auto sprite = level->ui_sprites.add();

		sprite->sort = 512;
		//sprite->scale = 0.5f;

		Component::attach(sprite, entity);

		sprites.push_back(sprite);
	}

	for (size_t i = 0; i < player->abilities.size(); ++i)
	{
		sprites[i]->sheet = player->abilities[i].ability->sheet;
	}
}
