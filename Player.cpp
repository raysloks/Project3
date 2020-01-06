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
		Vec2 direction = srs->screenToWorld(input->getCursor()) - entity->p;
		direction.Normalize();

		Entity entity;

		auto sheet = SpriteSheet::get("swoop.png");
		sheet->columns = 4;

		auto sprite = srs->sprites.add(Sprite(sheet));
		sprite->sort = -8;
		sprite->rotation = atan2f(direction.x, -direction.y) * 180.0f / float(M_PI);
		sprite->flip = flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
		entity.addComponent(sprite);

		auto animator = std::make_shared<SpriteAnimator>(30.0f);
		animator->destroy = true;
		entity.addComponent(&**cbs->behaviours.add(std::move(animator)));
		
		auto collider = cs->colliders.add(Collider());
		collider->shape = std::make_unique<SpriteShape>(sprite);
		collider->layers = 2;

		auto hit = std::make_shared<std::set<Enemy*>>();
		on_hit = [=](const Collision& collision)
		{
			if (auto enemy = collision.other->entity->getComponent<Enemy>())
			{
				if (hit->find(enemy) == hit->end())
				{
					sword->getComponent<SpriteAnimator>()->freeze = fmaxf(sword->getComponent<SpriteAnimator>()->freeze, 2.0f / 30.0f);
					enemy->entity->getComponent<SpriteAnimator>()->freeze = fmaxf(sword->getComponent<SpriteAnimator>()->freeze, 4.0f / 30.0f);
					enemy->entity->p += direction * 4.0f;
					hit->insert(enemy);
				}
			}
		};
		collider->callbacks.push_back(on_hit);

		entity.addComponent(collider);

		this->entity->addChild(&entity);

		sword = engine->add_entity(std::move(entity));

		flip = !flip;
	};
	input->addKeyDownCallback(KB_ATTACK, on_attack);
}

void Player::tick(float dt)
{
	if (!on_collision)
	{
		on_collision = [this](const Collision& collision)
			{
				entity->p -= collision.n * collision.pen;
				float v_dot_n = v.Dot(collision.n);
				if (v_dot_n > 0.0f)
					v -= collision.n * v_dot_n;

				n = collision.n;

				update_camera();
			};
		entity->getComponent<Collider>()->callbacks.push_back(on_collision);
	}

	auto sprite = entity->getComponent<Sprite>();

	if (input->isKeyPressed(SDLK_r))
		entity->p = Vec2(16.0f, 16.0f);

	float speed = 60.0f;
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
	move = Vec2(move.x + move.y, move.y - move.x);

	if (move != Vec2())
		facing = move.Normalized();

	if (move.x < move.y)
		sprite->subsprite_y = 1;
	if (move.x > move.y)
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

	update_camera();
}

void Player::update_camera()
{
	srs->camera_position.x = entity->p.x;
	srs->camera_position.y = entity->p.y;
}
