#include "Player.h"

#include <SDL.h>

#include "Collider.h"

#include "Enemy.h"

#include "GameKeyBinding.h"

#include <iostream>

#include "SpriteAnimator.h"

#include "SpriteShape.h"

#include "Tilemap.h"

Player::Player()
{
	hp = 100;
	hp_max = 100;

	on_attack = [this]()
	{
		Vec2 direction = srs->screenToWorld(input->getCursor()) - entity->p;
		direction.Normalize();

		Entity entity;

		auto swoop = SpriteSheet::get("swoop.png");
		swoop->columns = 4;

		auto swoop_iso = swoop->makeIsometricFloorLossless(atan2f(direction.x, -direction.y) * 180.0f / float(M_PI));

		auto sprite = srs->sprites.add(Sprite(swoop_iso));
		sprite->color = SDL_Color({ 0, 0, 0, 64 });
		sprite->sort = -64;
		//sprite->rotation = atan2f(direction.x, -direction.y) * 180.0f / float(M_PI);
		//sprite->flip = flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
		entity.addComponent(sprite);

		auto animator = std::make_shared<SpriteAnimator>(30.0f);
		animator->destroy = true;
		entity.addComponent(cbs->add(std::move(animator)));
		
		auto collider = cs->colliders.add(Collider());
		//collider->shape = std::make_unique<Circle>(10.0f);
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
					enemy->entity->p += direction * 20.0f;
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

	input->addKeyDownCallback(KB_ACTION_0, std::bind(&Player::onAction, this, 0));
	input->addKeyDownCallback(KB_ACTION_1, std::bind(&Player::onAction, this, 1));
	input->addKeyDownCallback(KB_ACTION_2, std::bind(&Player::onAction, this, 2));
	input->addKeyDownCallback(KB_ACTION_3, std::bind(&Player::onAction, this, 3));

	blood = 0;
}

void Player::tick(float dt)
{
	{
		auto current = tm->getEffect(entity->p);
		if (current > blood)
		{
			tm->setEffect(entity->p, current - 1);
			++blood;
		}
		else if (current < blood)
		{
			if (current == 0)
			{
				tm->setEffect(entity->p, current + 1);
				--blood;
			}
		}
	}

	auto sprite = entity->getComponent<Sprite>();

	if (input->isKeyPressed(SDLK_r))
		entity->p = Vec2(16.0f, 1600.0f);

	speed = 60.0f;
	acceleration = 120.0f;
	deceleration = 240.0f;

	move = Vec2();

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

	anim += v.Len() * dt * 0.5f;

	Mob::tick(dt);

	sprite->subsprite_x = (int(anim) % sprite->sheet->columns);

	update_camera();
}

void Player::onCollision(const Collision & collision)
{
	Mob::onCollision(collision);

	update_camera();
}

void Player::splatter()
{
	for (size_t i = 0; i < 50; ++i)
	{
		Vec2 p;
		p.x = rand() % 17 - 8 + rand() % 17 - 8 + rand() % 17 - 8;
		p.Rotate(rand() % 360);

		p += entity->p;

		uintmax_t current = tm->getEffect(p);
		if (current < 255)
			tm->setEffect(p, current + 1);
		else
			blood += 1;
	}
}

#include "Projectile.h"

void Player::onAction(size_t action)
{
	switch (action)
	{
	case 0:
		on_attack();
		break;
	case 1:
	{
		auto target = srs->screenToWorld(input->getCursor());
		auto in_range = cs->overlapCircle(target, 0.0f);
		if (in_range.empty())
			entity->p = target;
	}
	break;
	case 2:
		v += (srs->screenToWorld(input->getCursor()) - entity->p).Normalized() * 200.0f;
		splatter();
		break;
	case 3:
	{
		auto entity = engine->add_entity(Entity());
		entity->p = this->entity->p;

		Sprite sprite("ball.png");
		entity->addComponent(srs->sprites.add(std::move(sprite)));

		auto projectile = std::make_shared<Projectile>();
		projectile->v = srs->screenToWorld(input->getCursor()) - entity->p;
		entity->addComponent(cbs->add(projectile));

		auto collider = cs->colliders.add(Collider());
		entity->addComponent(collider);

		collider->shape = std::make_unique<Circle>(3.0f);
		collider->callbacks.push_back([projectile](const Collision& collision)
			{
				projectile->entity->p -= collision.n * collision.pen;
				float v_dot_n = projectile->v.Dot(collision.n);
				if (v_dot_n > 0.0f)
					projectile->v -= collision.n * v_dot_n * 2.0f;
			});
	}
	break;
	default:
		break;
	}
}

void Player::update_camera()
{
	srs->camera_position.x = entity->p.x;
	srs->camera_position.y = entity->p.y;
}
