#include "Enemy.h"

#include "Collider.h"

#include "CollisionSystem.h"

#include "Player.h"

void Enemy::tick(float dt)
{
	speed = 24.0f;
	acceleration = 9001.0f;
	deceleration = 9001.0f;

	move = Vec2();

	auto in_range = cs->overlapCircle(entity->p, 1000.0f);
	for (auto i : in_range)
	{
		auto player = i.second->entity->getComponent<Player>();
		if (player)
		{
			if (i.first > 10.0f)
			{
				move = player->entity->p - entity->p;
			}
			else
			{
				if (rand() % 30 == 0)
				{
					player->hp -= rand() % 10 + 1;
					player->splatter();
				}
			}
		}
	}

	Mob::tick(dt);

	auto sprite = entity->getComponent<Sprite>();
	if (sprite)
	{
		if (move.x < move.y)
			sprite->flip = SDL_FLIP_HORIZONTAL;
		if (move.x > move.y)
			sprite->flip = SDL_FLIP_NONE;
	}

	for (auto child : entity->getChildren())
	{
		auto child_sprite = child->getComponent<Sprite>();
		if (child_sprite)
			child_sprite->flip = sprite->flip;
	}
}