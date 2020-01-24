#include "Player.h"

#include <SDL.h>

#include "Collider.h"

#include "Enemy.h"

#include "GameKeyBinding.h"

#include <iostream>

#include "SpriteAnimator.h"

#include "Tilemap.h"

Player::Player()
{
	hp = 20;
	hp_max = 20;

	blood = 0;

	auto ability = std::make_shared<Ability>();
	ability->sheet = SpriteSheet::get("floor.png");

	AbilitySlot slot;
	slot.ability = ability;

	abilities.push_back(slot);
}

void Player::start()
{
	Mob::start();

	input->addKeyDownCallback(KB_ACTION_0, std::bind(&Player::onKey, this, 0));
	input->addKeyDownCallback(KB_ACTION_1, std::bind(&Player::onKey, this, 1));
	input->addKeyDownCallback(KB_ACTION_2, std::bind(&Player::onKey, this, 2));
	input->addKeyDownCallback(KB_ACTION_3, std::bind(&Player::onKey, this, 3));
	input->addKeyDownCallback(KB_ACTION_4, std::bind(&Player::onKey, this, 4));
	input->addKeyDownCallback(KB_ACTION_5, std::bind(&Player::onKey, this, 5));
	input->addKeyDownCallback(KB_ACTION_6, std::bind(&Player::onKey, this, 6));
	input->addKeyDownCallback(KB_ACTION_7, std::bind(&Player::onKey, this, 7));
	input->addKeyDownCallback(KB_ACTION_8, std::bind(&Player::onKey, this, 8));
	input->addKeyDownCallback(KB_ACTION_9, std::bind(&Player::onKey, this, 9));
}

void Player::tick(float dt)
{
	for (auto i = key_buffer.begin(); i != key_buffer.end(); )
	{
		i->first += dt;
		if (onAction(i->second) || i->first > 0.2f)
			i = key_buffer.erase(i);
		else
			++i;
	}

	/*for (size_t i = 0; i < 10; ++i)
		if (input->isKeyDown(KB_ACTION_0 + i))
			onAction(i);*/

	{
		auto current = tm->getEffect(entity->xy);
		if (current > blood && current > 1)
		{
			tm->setEffect(entity->xy, current - 1);
			++blood;
		}
		else if (current < blood)
		{
			if (current == 0)
			{
				tm->setEffect(entity->xy, current + 1);
				--blood;
			}
		}
		tm->refreshUpdatedEffects();
	}

	if (input->isKeyPressed(SDLK_r))
		entity->xy = Vec2(16.0f, 1600.0f);

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


	auto sprite = getComponent<Sprite>();
	if (sprite)
	{
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

		sprite->subsprite_x = (int(anim) % sprite->sheet->columns);
	}

	Mob::tick(dt);

	for (auto& ability : abilities)
	{
		ability.charges += dt / ability.ability->cooldown;
		if (ability.charges > ability.ability->max_charges)
			ability.charges = ability.ability->max_charges;
	}

	update_camera();
}

void Player::onCollision(const Collision & collision)
{
	if (entity == nullptr)
		return;

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

		p += entity->xy;

		uintmax_t current = tm->getEffect(p);
		if (current < 255)
			tm->setEffect(p, current + 1);
		else
			blood += 1;
	}

	tm->refreshUpdatedEffects();
}

#include "Projectile.h"

#include <algorithm>

void Player::onKey(size_t action)
{
	key_buffer.push_back(std::make_pair(0.0f, action));
}

bool Player::onAction(size_t action)
{
	if (cooldown > 0.0f)
		return false;
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
				target = enemy->entity->xy + (target - enemy->entity->xy).Normalized() * 8.0f;
				auto in_range = cs->overlapCircle(target, 0.0f, 1, [this](Collider * c) { return c->entity != entity; });
				if (in_range.empty())
				{
					entity->xy = target;
					enemy->onDamaged(rng->next(3) + 1);
					cooldown = 0.2f;
					return true;
				}
			}
		}
		return false;
	}
	case 1:
	{
		{
			Vec2 direction = srs->screenToWorld(input->getCursor()) - entity->xy;
			direction.Normalize();

			{
				auto sprite = entity->getComponent<Sprite>();

				if (direction.x > direction.y)
					sprite->flip = SDL_FLIP_NONE;
				if (direction.x < direction.y)
					sprite->flip = SDL_FLIP_HORIZONTAL;
			}

			sword = level->add_entity();
			sword->z = 4.0f;

			auto swoop = SpriteSheet::get("swoop_small.png");

			float rotation_degs = atan2f(direction.x, -direction.y) * 180.0f / float(M_PI);
			auto swoop_iso = swoop->makeIsometricFloorLossless(rotation_degs);

			auto sprite = level->sprites.add(Sprite(swoop_iso));
			sprite->color = SDL_Color({ 255, 255, 255, 255 });
			sprite->sort = direction.y * 8;
			//sprite->rotation = atan2f(direction.x, -direction.y) * 180.0f / float(M_PI);
			//sprite->flip = flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
			Component::attach(sprite, sword);

			auto animator = level->add<SpriteAnimator>(30.0f);
			animator->destroy = true;
			Component::attach(animator, sword);

			auto collider = level->sprite_colliders.add(sprite);
			collider->rotation = rotation_degs;
			collider->non_iso = swoop;
			collider->layers = 3;
			Component::attach(collider, sword);

			auto hit = std::make_shared<std::set<uint64_t>>();
			on_hit = [hit, this](const Collision& collision)
			{
				if (auto enemy = collision.other->entity->getComponent<Mob>())
				{
					if (enemy->entity == entity)
						return;
					auto guid = enemy->entity->getGUID();
					if (hit->find(guid) == hit->end())
					{
						if (entity)
						{
							auto animator = sword->getComponent<SpriteAnimator>();
							//animator->freeze = fmaxf(animator->freeze, 2.0f / 30.0f);
						}
						//enemy->entity->getComponent<SpriteAnimator>()->freeze = fmaxf(sword->getComponent<SpriteAnimator>()->freeze, 4.0f / 30.0f);
						//enemy->entity->p += direction * 20.0f;
						hit->insert(guid);
						enemy->onDamaged(rng->next(3) + 1);
					}
				}
			};
			collider->callbacks.push_back(on_hit);

			Entity::adopt(sword, this->entity);

			flip = !flip;
		}

		cooldown = 0.15f;
		return true;
	}
	case 2:
	{
		auto diff = srs->screenToWorld(input->getCursor()) - entity->xy;
		diff.Truncate(80.0f);
		auto target = entity->xy + diff;
		auto in_range = cs->overlapCircle(target, 0.0f, 1);
		if (in_range.empty())
		{
			// create poof
			{
				auto entity = level->add_entity();
				entity->xy = this->entity->xy;

				auto sprite = level->sprites.add("blink.png");
				sprite->sort = -8;
				sprite->sheet->columns = 4;
				Component::attach(sprite, entity);

				auto animator = level->add<SpriteAnimator>(30.0f);
				animator->destroy = true;
				Component::attach(animator, entity);
			}

			entity->xy = target;

			// create poof
			{
				auto entity = level->add_entity();
				entity->xy = this->entity->xy;

				auto sprite = level->sprites.add("blink.png");
				sprite->sort = -8;
				sprite->sheet->columns = 4;
				Component::attach(sprite, entity);

				auto animator = level->add<SpriteAnimator>(30.0f);
				animator->destroy = true;
				Component::attach(animator, entity);
			}

			cooldown = 0.2f;
			return true;
		}
		return false;
	}
	case 3:
	{
		auto diff = srs->screenToWorld(input->getCursor()) - entity->xy;
		auto dir = diff.Normalized();
		for (intmax_t i = -20; i <= 20; ++i)
		{
			for (intmax_t j = 0; j < 24 - llabs(i); ++j)
			{
				auto launch = dir * (j + rand() % 32 + rand() % 32);
				launch.Rotate(i);

				auto p = entity->xy + launch;

				uintmax_t current = tm->getEffect(p);
				if (current < 255)
					tm->setEffect(p, current + 1);
				else
					blood += 1;
			}
		}

		tm->refreshUpdatedEffects();

		cooldown = 0.2f;
		return true;
	}
	case 4:
	{
		auto target = srs->screenToWorld(input->getCursor());
		auto in_range = cs->overlapCircle(target, 8.0f);
		for (auto i : in_range)
		{
			auto enemy = i.second->getComponent<Enemy>();
			if (enemy)
			{
				auto diff = enemy->entity->xy - entity->xy;
				if (diff.Len() < 16.0f)
				{
					enemy->onDamaged(rng->next(3) + 1);
					cooldown = 0.2f;
					return true;
				}
			}
		}
		return false;
	}
	default:
		return false;
	}

	return false;
}

void Player::update_camera()
{
	srs->camera_position.x = entity->xy.x;
	srs->camera_position.y = entity->xy.y;
}
