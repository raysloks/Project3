#include "Enemy.h"

#include "Collider.h"

#include "CollisionSystem.h"

#include "Player.h"

void Enemy::tick(float dt)
{
	float speed = 24.0f;

	Vec2 move;

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

	float l = move.Len();
	if (l != 0.0f)
	{
		move /= l;

		move *= speed * dt;

		entity->p += move;
	}

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