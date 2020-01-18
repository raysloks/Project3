#include "BouncingBall.h"

void BouncingBall::start()
{
	auto collider = getComponent<CircleCollider>();
	if (collider)
		collider->callbacks.push_back(std::bind(&BouncingBall::onCollision, this, std::placeholders::_1));
}

void BouncingBall::tick(float dt)
{
	entity->xyz += v * dt;

	float az = -500;

	entity->z += az * 0.5f * dt * dt;

	v.z += az * dt;

	if (entity->z <= 4.0f)
	{
		//z = 0.0f; caused bounces to get bigger sometimes
		if (v.z < 0.0f)
			v.z = -v.z * elasticity;
	}

	update_shadow();
}

void BouncingBall::onCollision(const Collision & collision)
{
	entity->xy -= collision.n * collision.pen;
	float v_dot_n = v.Dot(collision.n);
	if (v_dot_n > 0.0f)
		v -= collision.n * v_dot_n * (1.0f + elasticity);

	update_shadow();
}

void BouncingBall::update_shadow()
{
	shadow->entity->xy = entity->xy;
}
