#include "CollisionSystem.h"

#include <iostream>

void CollisionSystem::tick(float dt)
{
	Collision collision;
	for (size_t i = 0; i < colliders.components.size() - 1; ++i)
	{
		auto& a = colliders.components[i];
		if (a.entity == nullptr)
			continue;

		for (size_t j = i + 1; j < colliders.components.size(); ++j)
		{
			auto& b = colliders.components[j];
			if (b.entity == nullptr)
				continue;

			Vec2 diff = b.entity->p - a.entity->p;

			if (a.shape->check(diff, b.shape.get(), collision))
			{
				collision.p += a.entity->p;
				collision.other = &b;
			
				for (auto i = a.callbacks.begin(); i != a.callbacks.end();)
				{
					auto callback = i->lock();
					if (callback)
					{
						(*callback)(collision);
						++i;
					}
					else
						a.callbacks.erase(i);
				}

				collision.flip();
				collision.other = &a;

				for (auto i = b.callbacks.begin(); i != b.callbacks.end();)
				{
					auto callback = i->lock();
					if (callback)
					{
						(*callback)(collision);
						++i;
					}
					else
						b.callbacks.erase(i);
				}
			}
		}
	}
}

#include "Circle.h"

std::map<float, Collider*> CollisionSystem::overlapCircle(const Vec2& p, float r)
{
	std::map<float, Collider*> ret;

	Circle circle(r);

	Collision collision;

	for (size_t i = 0; i < colliders.components.size(); ++i)
	{
		auto& a = colliders.components[i];
		if (a.entity == nullptr)
			continue;

		Vec2 diff = a.entity->p - p;
		
		if (circle.check(diff, a.shape.get(), collision))
			ret.insert(std::make_pair(r - collision.pen, &a));
	}

	return ret;
}
