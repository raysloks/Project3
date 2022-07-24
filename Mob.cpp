#include "Mob.h"

#include "FloatingText.h"

#include "StatContext.h"

Mob::Mob()
{
	hp = 3;

	cooldown = 0.0f;

	for (size_t i = 0; i < stats.stats_array.size(); ++i)
		stats.stats_array[i] = 0;
}

void Mob::start()
{
	auto collider = getComponent<CircleCollider>();
	if (collider)
		collider->callbacks.push_back(std::bind(&Mob::onCollision, this, std::placeholders::_1));
}

void Mob::tick(float dt)
{
}

void Mob::onCollision(const Collision & collision)
{
	if (entity == nullptr)
		return;

	if (collision.other->entity->getRoot() == entity->getRoot())
		return;

	entity->xy -= collision.n * collision.pen;
	float v_dot_n = v.Dot(collision.n);
	if (v_dot_n > 0.0f)
		v -= collision.n * v_dot_n;

	n = collision.n;
}

void Mob::onDamaged(int64_t damage)
{
	hp.current -= damage;

	// create floating text
	{
		auto entity = level->add_entity();
		entity->xy = this->entity->xy;
		entity->z = this->entity->z + 8;

		auto text = level->add<FloatingText>(std::to_string(damage), 32, 1);

		text->v.xy = Vec2(16.0f, 0.0f).Rotate(rng->next_float() * 360.0f);
		text->v.z = -16.0f;

		text->a.z = 128.0f;

		Component::attach(text, entity);
	}

	/*{
		auto diff = srs->screenToWorld(input->getCursor()) - entity->xy;
		auto dir = diff.Normalized();
		size_t total_remaining = 0;
		for (intmax_t i = -20; i <= 20; ++i)
		{
			size_t remaining = 1;
			for (intmax_t j = 0; j < 24 - llabs(i) || remaining > 0; ++j)
			{
				if (j < 24 - llabs(i))
					remaining += 2;

				auto launch = dir * (j + (rng->next_float() + rng->next_float()) * 32.0f);
				launch.Rotate(i);

				auto p = entity->xy + launch;

				tm->depositEffect(0, remaining, p);
			}
			total_remaining += remaining;
		}

		tm->refreshUpdatedEffects();
	}*/

	/*if (hp <= 0)
		level->remove_entity(entity);*/
}

void Mob::recalculateStats()
{
	StatContext stat_context{ 0 };
	stat_cache = stats.calculateStats(stat_context);
}

float Mob::getMovementSpeed() const
{
	return stat_cache.movement_speed * STAT_UNIT_FACTOR;
}
