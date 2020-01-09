#include "Mob.h"

void Mob::start()
{
	auto collider = getComponent<CircleCollider>();
	if (collider)
		collider->callbacks.push_back(std::bind(&Mob::onCollision, this, std::placeholders::_1));
}

void Mob::tick(float dt)
{
	entity->p += v * dt;

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
	entity->p += at * 0.5f * dt;
}

void Mob::onCollision(const Collision & collision)
{
	entity->p -= collision.n * collision.pen;
	float v_dot_n = v.Dot(collision.n);
	if (v_dot_n > 0.0f)
		v -= collision.n * v_dot_n;

	n = collision.n;
}

void Mob::onDamaged(int64_t damage)
{
	hp -= damage;

	if (hp <= 0)
		engine->remove_entity(entity);
}
