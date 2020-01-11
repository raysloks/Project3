#include "CollisionSystem.h"

#include <iostream>

#include <algorithm>

void callCallbacks(Collider& a, Collider& b, Collision& collision)
{
	collision.other = &b;

	for (auto i = a.callbacks.begin(); i != a.callbacks.end();)
	{
		auto callback = *i;
		if (callback)
		{
			callback(collision);
			++i;
		}
		else
			a.callbacks.erase(i);
	}

	collision.flip();
	collision.other = &a;

	for (auto i = b.callbacks.begin(); i != b.callbacks.end();)
	{
		auto callback = *i;
		if (callback)
		{
			callback(collision);
			++i;
		}
		else
			b.callbacks.erase(i);
	}
}

void CollisionSystem::tick(float dt)
{
	std::vector<Collision> collisions;
	collisions.reserve(256);

	// TODO make a component class for each collider shape to avoid virtual function calls and 'flipping'
	/*for (size_t i = 0; i < colliders.components.size() - 1; ++i)
	{
		auto& a = colliders.components[i];
		if (a.entity == nullptr)
			continue;

		for (size_t j = i + 1; j < colliders.components.size(); ++j)
		{
			auto& b = colliders.components[j];
			if (b.entity == nullptr)
				continue;

			if ((a.layers & b.layers) == 0)
				continue;

			size_t count = 0;
			do
			{
				Vec2 diff = b.entity->getPosition() - a.entity->getPosition();

				collisions.clear();

				a.shape->check(diff, b.shape.get(), collisions);

				if (collisions.empty())
					break;

				std::sort(collisions.begin(), collisions.end(), [](auto a, auto b) { return a.pen > b.pen; });

				auto& collision = collisions.front();

				collision.p += a.entity->p;

				callCallbacks(a, b, collision);
				
			} while (collisions.size() > 1 && ++count < 2);
		}
	}*/

	for (size_t i = 0; i < circles.components.size(); ++i)
	{
		auto& a = circles.components[i];
		if (a.entity == nullptr)
			continue;

		for (size_t j = i + 1; j < circles.components.size(); ++j)
		{
			auto& b = circles.components[j];
			if (b.entity == nullptr)
				continue;

			if ((a.layers & b.layers) == 0)
				continue;

			collisions.clear();

			Vec2 diff = b.entity->getPosition() - a.entity->getPosition();

			a.check(diff, b, collisions);

			if (collisions.empty())
				continue;

			auto& collision = collisions.front();

			collision.p += a.entity->p;

			callCallbacks(a, b, collision);
		}
	}

	for (size_t i = 0; i < rectangles.components.size(); ++i)
	{
		auto& a = rectangles.components[i];
		if (a.entity == nullptr)
			continue;

		for (size_t j = 0; j < circles.components.size(); ++j)
		{
			auto& b = circles.components[j];
			if (b.entity == nullptr)
				continue;

			if ((a.layers & b.layers) == 0)
				continue;

			collisions.clear();

			Vec2 diff = b.entity->getPosition() - a.entity->getPosition();

			a.check(diff, b, collisions);

			if (collisions.empty())
				continue;

			auto& collision = collisions.front();

			collision.p += a.entity->p;

			callCallbacks(a, b, collision);
		}
	}

	for (size_t i = 0; i < tilemaps.components.size(); ++i)
	{
		auto& a = tilemaps.components[i];
		if (a.entity == nullptr)
			continue;

		for (size_t j = 0; j < circles.components.size(); ++j)
		{
			auto& b = circles.components[j];
			if (b.entity == nullptr)
				continue;

			if ((a.layers & b.layers) == 0)
				continue;
			
			size_t count = 0;
			do
			{
				Vec2 diff = b.entity->getPosition() - a.entity->getPosition();

				collisions.clear();

				a.check(diff, b, collisions);

				if (collisions.empty())
					break;

				std::sort(collisions.begin(), collisions.end(), [](auto a, auto b) { return a.pen > b.pen; });

				auto& collision = collisions.front();

				collision.p += a.entity->p;

				callCallbacks(a, b, collision);
				
			} while (collisions.size() > 1 && ++count < 2);
		}
	}
}

#include "Circle.h"

std::map<float, Collider*> CollisionSystem::overlapCircle(const Vec2& p, float r, const std::function<bool(Collider*)>& filter)
{
	std::map<float, Collider*> ret;

	CircleCollider circle(r);

	std::vector<Collision> collisions;
	collisions.reserve(256);

	for (size_t i = 0; i < circles.components.size(); ++i)
	{
		auto& a = circles.components[i];
		if (a.entity == nullptr)
			continue;

		if (filter)
			if (!filter(&a))
				continue;

		Vec2 diff = p - a.entity->p;

		size_t j = collisions.size();
		a.check(diff, circle, collisions);
		for (; j < collisions.size(); ++j)
		{
			ret.insert(std::make_pair(r - collisions[j].pen, &a));
		}
	}

	for (size_t i = 0; i < rectangles.components.size(); ++i)
	{
		auto& a = rectangles.components[i];
		if (a.entity == nullptr)
			continue;

		if (filter)
			if (!filter(&a))
				continue;

		Vec2 diff = p - a.entity->p;

		size_t j = collisions.size();
		a.check(diff, circle, collisions);
		for (; j < collisions.size(); ++j)
		{
			ret.insert(std::make_pair(r - collisions[j].pen, &a));
		}
	}

	for (size_t i = 0; i < tilemaps.components.size(); ++i)
	{
		auto& a = tilemaps.components[i];
		if (a.entity == nullptr)
			continue;

		if (filter)
			if (!filter(&a))
				continue;

		Vec2 diff = p - a.entity->p;

		size_t j = collisions.size();
		a.check(diff, circle, collisions);
		for (; j < collisions.size(); ++j)
		{
			ret.insert(std::make_pair(r - collisions[j].pen, &a));
		}
	}

	return ret;
}
