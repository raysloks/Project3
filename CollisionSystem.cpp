#include "CollisionSystem.h"

#include <iostream>

#include <algorithm>

void CollisionSystem::tick(float dt)
{
	std::vector<Collision> collisions;
	collisions.reserve(256);

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

			size_t count = 0;
			do
			{
				Vec2 diff = b.entity->p - a.entity->p;

				collisions.clear();

				a.shape->check(diff, b.shape.get(), collisions);

				if (collisions.empty())
					break;

				std::sort(collisions.begin(), collisions.end(), [](auto a, auto b) { return a.pen > b.pen; });

				auto& collision = collisions.front();

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
			} while (collisions.size() > 1 && ++count < 2);
		}
	}
}

#include "Circle.h"

std::map<float, Collider*> CollisionSystem::overlapCircle(const Vec2& p, float r)
{
	std::map<float, Collider*> ret;

	Circle circle(r);

	std::vector<Collision> collisions;
	collisions.reserve(256);

	for (size_t i = 0; i < colliders.components.size(); ++i)
	{
		auto& a = colliders.components[i];
		if (a.entity == nullptr)
			continue;

		Vec2 diff = a.entity->p - p;
		
		size_t j = collisions.size();
		circle.check(diff, a.shape.get(), collisions);
		for (; j < collisions.size(); ++j)
		{
			ret.insert(std::make_pair(r - collisions[i].pen, &a));
		}
	}

	return ret;
}
