#include "NetworkMob.h"

#include "MobTemplate.h"

NetworkMob::NetworkMob()
{
}

void NetworkMob::start()
{
}

void NetworkMob::tick(float dt)
{
	//Mob::tick(dt);

	auto sprite = entity->getComponent<Sprite>();
	if (sprite)
	{
		if (move.x > move.y)
			sprite->flip = SDL_FLIP_NONE;
		if (move.x < move.y)
			sprite->flip = SDL_FLIP_HORIZONTAL;

		anim += v.Len() * dt * 0.8f;
		if (v == Vec2())
		{
			anim = 0;
			sprite->subsprite_y = 0;
		}
		else
		{
			sprite->subsprite_y = 1;
		}

		sprite->subsprite_x = (int(anim) % sprite->sheet->columns);
	}

	/*for (auto child : entity->getChildren())
	{
		auto child_sprite = child->getComponent<Sprite>();
		if (child_sprite)
			child_sprite->flip = sprite->flip;
	}*/

	entity->z = tm->getZ(entity->xy);
}

#include <iostream>

void NetworkMob::setMobTemplate(uint64_t new_mob_template_id)
{
	mob_template_id = new_mob_template_id;
	mob_template = MobTemplate::get(mob_template_id);
	memcpy(&base_stats, &mob_template->stats, sizeof(MobStatBlock));
	recalculateStats();

	hp.set_cap(stats.hp);
}
