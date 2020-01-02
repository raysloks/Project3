#include "Enemy.h"

#include "Collider.h"

#include "CollisionSystem.h"

#include "Player.h"

void Enemy::tick(float dt)
{
	if (!on_collision)
	{
		on_collision = std::make_shared<std::function<void(const Collision&)>>([this](const Collision& collision)
			{
				entity->p -= collision.n * collision.pen;
			});
		entity->getComponent<Collider>()->callbacks.push_back(on_collision);
	}

	float speed = 240.0f;

	Vec2 move;

	auto in_range = cs->overlapCircle(entity->p, 100.0f);
	for (auto i : in_range)
	{
		auto player = dynamic_cast<Player*>(i.second->entity->getComponent<CustomBehaviour>());
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
}