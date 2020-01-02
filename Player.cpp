#include "Player.h"

#include <SDL.h>

#include "Collider.h"

#include "Enemy.h"

#include "GameKeyBinding.h"

#include <iostream>

#include "SpriteAnimator.h"

#include "SpriteShape.h"

Player::Player()
{
	on_attack = [this]()
	{
		Entity entity;

		auto sheet = Spritesheet::get("swoop.png");
		sheet->columns = 4;

		Sprite sprite(sheet);
		sprite.sort = -8;
		sprite.rotation = atan2f(facing.x, -facing.y) * 180.0f / float(M_PI);
		sprite.flip = flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
		entity.addComponent(srs->sprites.add(std::move(sprite)));

		auto animator = std::make_shared<SpriteAnimator>(30.0f);
		animator->destroy = true;
		entity.addComponent(&**cbs->behaviours.add(std::move(animator)));

		Collider collider;
		auto sprite_shape = std::make_unique<SpriteShape>();
		sword_shape = sprite_shape.get();
		collider.shape = std::move(sprite_shape);

		on_hit = [this](const Collision& collision)
		{
			if (auto enemy = collision.other->entity->getComponent<Enemy>())
			{
				enemy->entity->p += collision.n * collision.pen + facing * 16.0f;
			}
		};
		collider.callbacks.push_back(on_hit);

		entity.addComponent(cs->colliders.add(std::move(collider)));

		this->entity->addChild(&entity);

		sword = engine->add_entity(std::move(entity));

		flip = !flip;
	};
	input->addKeyDownCallback(KB_ATTACK, on_attack);

	{
		Entity entity;

		Sprite sprite("shadow.png");
		sprite.sort = -16;
		entity.addComponent(engine->srs->sprites.add(std::move(sprite)));

		engine->add_entity(std::move(entity));
	}
}

void Player::tick(float dt)
{
	if (!on_collision)
	{
		on_collision = [this](const Collision& collision)
			{
				entity->p -= collision.n * collision.pen;
				v -= collision.n * v.Dot(collision.n);

				n = collision.n;

				update_followers();
			};
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
	srs->camera_position.x = entity->p.x;
	srs->camera_position.y = entity->p.y;

	if (auto entity = engine->get_entity(sword))
	{
		if (auto sprite = entity->getComponent<Sprite>())
		{
			if (sword_shape)
			{
				sword_shape->sprite = sprite;
			}
		}
	}
}
