#include "Mob.h"

Mob::Mob()
{
	hp = 3;
	hp_max = 3;

	cooldown = 0.0f;
}

void Mob::start()
{
	auto collider = getComponent<CircleCollider>();
	if (collider)
		collider->callbacks.push_back(std::bind(&Mob::onCollision, this, std::placeholders::_1));
}

void Mob::tick(float dt)
{
	entity->xy += v * dt;

	Vec2 v_prev = v;

	float n_dot_move = n.Dot(move);
	if (n_dot_move > 0.0f)
		move -= n * n_dot_move;

	n = Vec2();

	float l = move.Len();
	if (l != 0.0f)
	{
		if (l > 1.0f)
			move /= l;

		v += move * (acceleration + deceleration) * dt;
	}

	float mag = v.Len();
	float loss = deceleration * dt;
	float over = mag - loss - speed;
	if (over > 0.0f)
		loss += fminf(over, loss);

	if (loss < mag)
		v -= v / mag * loss;
	else
		v = Vec2();

	Vec2 at = v - v_prev;
	entity->xy += at * 0.5f * dt;

	entity->z = tm->getZ(entity->xy);

	cooldown -= dt;
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

#include "FloatingText.h"

void Mob::onDamaged(int64_t damage)
{
	hp -= damage;

	// create floating text
	{
		auto entity = level->add_entity();
		entity->xy = this->entity->xy;

		auto text = level->add<FloatingText>(std::to_string(damage), 32, 1);
		Component::attach(text, entity);
	}

	if (hp <= 0)
		level->remove_entity(entity);
}
