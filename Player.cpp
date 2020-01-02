#include "Player.h"

#include <SDL.h>

#include "Collider.h"

#include "Enemy.h"

#include "GameKeyBinding.h"

#include <iostream>

#include "SpriteAnimator.h"

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
					enemy->entity->removeComponent(enemy);
				}
			}

			{
				Entity entity;

				Sprite sprite("swoop.png");
				sprite.sheet->columns = 4;
				sprite.sort = -8;
				sprite.rotation = -atan2f(facing.x, facing.y) * 180.0f / float(M_PI);
				sprite.flip = flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
				entity.addComponent(engine->srs->sprites.add(std::move(sprite)));

				auto animator = std::make_shared<SpriteAnimator>(30.0f);
				animator->destroy = true;
				entity.addComponent(&**engine->cbs->behaviours.add(std::move(animator)));

				this->entity->addChild(&entity);
				
				engine->add_entity(std::move(entity));

				flip = !flip;
			}
		});
	input->addKeyDownCallback(KB_ATTACK, on_attack);

	{
		Entity entity;

		Sprite sprite("shadow.png");
		sprite.sort = -16;
		entity.addComponent(engine->srs->sprites.add(std::move(sprite)));

		shadow = engine->add_entity(std::move(entity));
	}
}

void Player::tick(float dt)
{
	if (!on_collision)
	{
		on_collision = std::make_shared<std::function<void(const Collision&)>>([this](const Collision& collision)
			{
				entity->p -= collision.n * collision.pen;
				v -= collision.n * v.Dot(collision.n);

				n = collision.n;

				update_followers();
			});
		entity->getComponent<Collider>()->callbacks.push_back(on_collision);
	}

	auto sprite = entity->getComponent<Sprite>();

	if (input->isKeyPressed(SDLK_r))
		entity->p = Vec2(16.0f, 16.0f);

	float speed = 120.0f;
	float acceleration = 120.0f;
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

	if (move != Vec2())
		facing = move.Normalized();

	if (move.x < 0.0f)
		sprite->subsprite_y = 1;
	if (move.x > 0.0f)
		sprite->subsprite_y = 0;

	float n_dot_move = n.Dot(move);
	if (n_dot_move > 0.0f)
		move -= n * n_dot_move;
	
	n = Vec2();

	entity->p += v * dt;

	Vec2 v_prev = v;

	float l = move.Len();
	if (l != 0.0f)
	{
		move /= l;

		v += move * (acceleration + deceleration) * dt;
	}

	v -= v.Truncated(deceleration * dt);
	v.Truncate(speed);

	Vec2 at = v - v_prev;
	entity->p += at * 0.5f * dt;

	anim += v.Len() * dt * 0.5f;

	sprite->subsprite_x = (int(anim) % sprite->sheet->columns);

	update_followers();
}

void Player::update_followers()
{
	auto shadow_entity = engine->get_entity(shadow);
	shadow_entity->p = entity->p;

	srs->camera_position.x = entity->p.x;
	srs->camera_position.y = entity->p.y;
}
