#include "NetworkMob.h"

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

	for (auto child : entity->getChildren())
	{
		auto child_sprite = child->getComponent<Sprite>();
		if (child_sprite)
			child_sprite->flip = sprite->flip;
	}
}
