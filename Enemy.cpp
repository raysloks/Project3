#include "Enemy.h"

#include "Collider.h"

#include "CollisionSystem.h"

#include "Player.h"

Enemy::Enemy()
{
	hp = 10;
	
	base_stats.move_speed = 15.0f;
	/*base_stats.move_acc = 30.0f;
	base_stats.move_dec = 30.0f;*/
}

void Enemy::start()
{
	Mob::start();
}

void Enemy::tick(float dt)
{
	move = Vec2();

	auto in_range = cs->overlapCircle(entity->xy, 64.0f);
	for (auto i : in_range)
	{
		auto player = i.second->entity->getComponent<Player>();
		if (player)
		{
			if (i.first > 8.0f)
			{
				move = player->entity->xy - entity->xy;
			}
			else if (cooldown <= 0.0f)
			{
				cooldown = 3.0f;
			}
		}
	}

	Mob::tick(dt);

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