#include "CollisionSystem.h"

#include <iostream>

void CollisionSystem::tick(float dt)
{
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

			Vec2 diff = a.entity->p - b.entity->p;
			float distance = diff.Len();
			Vec2 dir = diff / distance;
			float r_sum = a.r + b.r;
			if (distance < r_sum && distance != 0.0f)
			{
				for (auto i = a.callbacks.begin(); i != a.callbacks.end();)
				{
					auto callback = i->lock();
					if (callback)
					{
						(*callback)(Collision{ a.entity->p - dir * a.r, -dir, r_sum - distance, &b });
						++i;
					}
					else
						a.callbacks.erase(i);
				}
				for (auto i = b.callbacks.begin(); i != b.callbacks.end();)
				{
					auto callback = i->lock();
					if (callback)
					{
						(*callback)(Collision{ b.entity->p + dir * b.r, dir, r_sum - distance, &a });
						++i;
					}
					else
						b.callbacks.erase(i);
				}
			}
		}
	}
}

std::map<float, Collider*> CollisionSystem::overlapCircle(Vec2 p, float r)
{
	std::map<float, Collider*> ret;

	for (size_t i = 0; i < colliders.components.size(); ++i)
	{
		auto& a = colliders.components[i];
		if (a.entity == nullptr)
			continue;
		Vec2 diff = a.entity->p - p;
		float distance = diff.Len();
		float r_sum = a.r + r;
		if (distance < r)
			ret.insert(std::make_pair(distance, &a));
	}

	return ret;
}
