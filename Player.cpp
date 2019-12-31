#include "Player.h"

#include <SDL.h>

#include "Collider.h"

#include "Enemy.h"

#include <iostream>

Player::Player()
{
	on_attack = std::make_shared<std::function<void(void)>>([this]()
		{
			auto in_range = cs->overlapCircle(entity->p + v * 1.0f, 6.0f);
			if (in_range.empty())
			{
				entity->p += v * 1.0f;
			}
		});
	input->addKeyDownCallback(1 | (1ull << 32), on_attack);
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

	float speed = 60.0f;
	float acceleration = 240.0f;
	float deceleration = 480.0f;

	Vec2 move;

	if (input->isKeyDown(SDLK_w))
		move.y -= 1.0f;
	if (input->isKeyDown(SDLK_a))
		move.x -= 1.0f;
	if (input->isKeyDown(SDLK_s))
		move.y += 1.0f;
	if (input->isKeyDown(SDLK_d))
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
