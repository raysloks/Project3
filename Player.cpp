#include "Player.h"

#include <SDL.h>

#include "Collider.h"

#include "Enemy.h"

#include "GameKeyBinding.h"

#include <iostream>

Player::Player()
{
	on_attack = std::make_shared<std::function<void(void)>>([this]()
		{
			auto in_range = cs->overlapCircle(entity->p, 16.0f);

			for (auto i : in_range)
			{
				auto enemy = i.second->entity->getComponent<Enemy>();
				if (enemy)
				{
					engine->remove_entity(enemy->entity);
				}
			}
		});
	input->addKeyDownCallback(KB_ATTACK, on_attack);
}

void Player::tick(float dt)
{
	if (!on_collision)
	{
		on_collision = std::make_shared<std::function<void(const Collision&)>>([this](const Collision& collision)
			{
				entity->p -= collision.n * collision.pen;
				v -= collision.n * v.Dot(collision.n);

				srs->camera_position.x = entity->p.x;
				srs->camera_position.y = entity->p.y;
			});
		entity->getComponent<Collider>()->callbacks.push_back(on_collision);
	}

	if (input->isKeyPressed(SDLK_r))
		entity->p = Vec2(16.0f, 16.0f);

	float speed = 60.0f;
	float acceleration = 240.0f;
	float deceleration = 480.0f;

	Vec2 move;

	if (input->isKeyDown(KB_UP))
		move.y -= 1.0f;
	if (input->isKeyDown(KB_LEFT))
		move.x -= 1.0f;
	if (input->isKeyDown(KB_DOWN))
		move.y += 1.0f;
	if (input->isKeyDown(KB_RIGHT))
		move.x += 1.0f;

	auto sprite = entity->getComponent<Sprite>();


	float l = move.Len();
	if (l != 0.0f)
	{
		move /= l;

		v += move * (acceleration + deceleration) * dt;

		if (move.x < 0.0f)
			sprite->subsprite_y = 1;
		if (move.x > 0.0f)
			sprite->subsprite_y = 0;
	}

	v -= v.Truncated(deceleration * dt);
	v.Truncate(speed);

	entity->p += v * dt;

	anim += v.Len() * dt * 0.5f;

	sprite->subsprite_x = (int(anim) % sprite->sheet->columns);

	srs->camera_position.x = entity->p.x;
	srs->camera_position.y = entity->p.y;
}
