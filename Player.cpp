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

	speed = 25.0f;
	acceleration = 50.0f;
	deceleration = 200.0f;

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

	if (move.x > move.y)
		sprite->flip = SDL_FLIP_NONE;
	if (move.x < move.y)
		sprite->flip = SDL_FLIP_HORIZONTAL;

	anim += v.Len() * dt * 0.8f;
	if (v == Vec2())
	{
		anim = 3;
		sprite->subsprite_y = 0;
	}
	else
	{
		sprite->subsprite_y = 1;
	}

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

#include <algorithm>

void Player::onAction(size_t action)
{
	switch (action)
	{
	case 0:
	{
		auto target = srs->screenToWorld(input->getCursor());
		auto in_range = cs->overlapCircle(target, 8.0f);
		for (auto i : in_range)
		{
			auto enemy = i.second->getComponent<Enemy>();
			if (enemy)
			{
				target = enemy->entity->p + (target - enemy->entity->p).Normalized() * 8.0f;
				auto in_range = cs->overlapCircle(target, 0.0f, [this](Collider * c) { return c->entity != entity; });
				if (in_range.empty())
				{
					entity->p = target;
					enemy->onDamaged(10);
					break;
				}
			}
		}
	}
	break;
	case 1:
	{
		on_attack();
	}
	break;
	case 2:
	{
		auto diff = srs->screenToWorld(input->getCursor()) - entity->p;
		diff.Truncate(160.0f);
		auto target = entity->p + diff;
		auto in_range = cs->overlapCircle(target, 0.0f);
		if (in_range.empty())
		{
			// create poof
			{
				Entity entity;
				entity.p = this->entity->p;

				Sprite sprite("blink.png");
				sprite.sort = -8;
				sprite.sheet->columns = 4;
				entity.addComponent(srs->sprites.add(std::move(sprite)));

				auto animator = std::make_shared<SpriteAnimator>(30.0f);
				animator->destroy = true;
				entity.addComponent(cbs->add(std::move(animator)));

				engine->add_entity(std::move(entity));
			}

			entity->p = target;

			// create poof
			{
				Entity entity;
				this->entity->addChild(&entity);

				Sprite sprite("blink.png");
				sprite.sort = -8;
				sprite.sheet->columns = 4;
				entity.addComponent(srs->sprites.add(std::move(sprite)));

				auto animator = std::make_shared<SpriteAnimator>(30.0f);
				animator->destroy = true;
				entity.addComponent(cbs->add(std::move(animator)));

				engine->add_entity(std::move(entity));
			}
		}
	}
	break;
	case 3:
	{
		auto entity = engine->add_entity(Entity());
		entity->p = this->entity->p;

		Sprite sprite("shadow4_iso.png");
		entity->addComponent(srs->sprites.add(std::move(sprite)));

		auto projectile = std::make_shared<Projectile>();
		projectile->v = srs->screenToWorld(input->getCursor()) - entity->p;
		entity->addComponent(cbs->add(projectile));

		auto collider = cs->circles.add(CircleCollider(2.0f));
		entity->addComponent(collider);

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
