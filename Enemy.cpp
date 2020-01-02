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
		if (player && i.first > 10.0f)
		{
			move = player->entity->p - entity->p;
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
		if (move.x < 0.0f)
			sprite->subsprite_y = 1;
		if (move.x > 0.0f)
			sprite->subsprite_y = 0;
	}
}