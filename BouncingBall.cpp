#include "BouncingBall.h"

void BouncingBall::tick(float dt)
{
	if (!on_collision)
	{
		on_collision = [this](const Collision& collision)
		{
			entity->p -= collision.n * collision.pen;
			float v_dot_n = v.Dot(collision.n);
			if (v_dot_n > 0.0f)
				v -= collision.n * v_dot_n * 2.0f;

			update_visual();
		};
		entity->getComponent<Collider>()->callbacks.push_back(on_collision);
	}

	entity->p += v * dt;

	float az = -500;

	z += az * 0.5f * dt * dt;
	z += vz * dt;

	vz += az * dt;

	if (z <= 0.0f)
	{
		//z = 0.0f; caused bounces to get bigger sometimes
		if (vz < 0.0f)
			vz = -vz;
	}

	update_visual();
}

void BouncingBall::update_visual()
{
	float h = z + 4;
	visual->entity->p = entity->p + Vec2(-1.0f, -1.0f) * h;
	visual->sort = h * 2.0f;
}
