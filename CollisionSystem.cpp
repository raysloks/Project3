#include "CollisionSystem.h"

#include <iostream>

#include <algorithm>

#include "CircleCollider.h"
#include "RectangleCollider.h"
#include "SpriteCollider.h"
#include "TilemapCollider.h"

#include "Level.h"

void callCallbacks(Collider& a, Collider& b, Collision& collision)
{
	collision.collider = &a;
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

	auto & circles = level->circle_colliders;
	auto & rectangles = level->rectangle_colliders;
	auto & tilemaps = level->tilemap_colliders;
	auto & sprites = level->sprite_colliders;

	for (size_t i = 0; i < circles.components.size(); ++i)
	{
		auto& a = circles.components[i];
		if (a.entity == nullptr)
		{
			a.entity = Reference<Entity>(nullptr, 1); // hacky way to keep track of which components have been removed
			circles.remove(i);
			continue;
		}
		if (!a.entity)
			continue;

		for (size_t j = i + 1; j < circles.components.size(); ++j)
		{
			auto& b = circles.components[j];
			if (!b.entity)
				continue;

			if ((a.layers & b.layers) == 0)
				continue;

			collisions.clear();

			Vec2 diff = b.entity->getPosition() - a.entity->getPosition();

			a.check(diff, b, collisions);

			if (collisions.empty())
				continue;

			auto& collision = collisions.front();

			collision.p += a.entity->xy;

			callCallbacks(a, b, collision);
		}
	}

	for (size_t i = 0; i < rectangles.components.size(); ++i)
	{
		auto& a = rectangles.components[i];
		if (a.entity == nullptr)
		{
			a.entity = Reference<Entity>(nullptr, 1); // hacky way to keep track of which components have been removed
			rectangles.remove(i);
			continue;
		}
		if (!a.entity)
			continue;

		for (size_t j = 0; j < circles.components.size(); ++j)
		{
			auto& b = circles.components[j];
			if (!b.entity)
				continue;

			if ((a.layers & b.layers) == 0)
				continue;

			collisions.clear();

			Vec2 diff = b.entity->getPosition() - a.entity->getPosition();

			a.check(diff, b, collisions);

			if (collisions.empty())
				continue;

			auto& collision = collisions.front();

			collision.p += a.entity->xy;

			callCallbacks(a, b, collision);
		}
	}

	for (size_t i = 0; i < tilemaps.components.size(); ++i)
	{
		auto& a = tilemaps.components[i];
		if (a.entity == nullptr)
		{
			a.entity = Reference<Entity>(nullptr, 1); // hacky way to keep track of which components have been removed
			tilemaps.remove(i);
			continue;
		}
		if (!a.entity)
			continue;

		for (size_t j = 0; j < circles.components.size(); ++j)
		{
			auto& b = circles.components[j];
			if (!b.entity)
				continue;

			if ((a.layers & b.layers) == 0)
				continue;
			
			// check twice to prevent clipping into concave corners
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

				collision.p += a.entity->xy;

				callCallbacks(a, b, collision);
				
			} while (collisions.size() > 1 && ++count < 2);
		}
	}

	for (size_t i = 0; i < sprites.components.size(); ++i)
	{
		auto& a = sprites.components[i];
		if (a.entity == nullptr)
		{
			a.entity = Reference<Entity>(nullptr, 1); // hacky way to keep track of which components have been removed
			sprites.remove(i);
			continue;
		}
		if (!a.entity)
			continue;

		for (size_t j = 0; j < circles.components.size(); ++j)
		{
			auto& b = circles.components[j];
			if (!b.entity)
				continue;

			if ((a.layers & b.layers) == 0)
				continue;

			collisions.clear();

			Vec2 diff = b.entity->getPosition() - a.entity->getPosition();

			a.check(diff, b, collisions);

			if (collisions.empty())
				continue;

			auto& collision = collisions.front();

			collision.p += a.entity->xy;

			callCallbacks(a, b, collision);
		}
	}
}

std::map<float, Collider*> CollisionSystem::overlapCircle(const Vec2& p, float r, uint64_t layers, const std::function<bool(Collider*)>& filter)
{
	std::map<float, Collider*> ret;

	CircleCollider circle(r);

	std::vector<Collision> collisions;
	collisions.reserve(256);

	auto & circles = level->circle_colliders.components;
	auto & rectangles = level->rectangle_colliders.components;
	auto & tilemaps = level->tilemap_colliders.components;
	auto & sprites = level->sprite_colliders.components;

	for (size_t i = 0; i < circles.size(); ++i)
	{
		auto& a = circles[i];
		if (!a.entity)
			continue;

		if ((a.layers & layers) == 0)
			continue;

		if (filter)
			if (!filter(&a))
				continue;

		Vec2 diff = p - a.entity->xy;

		size_t j = collisions.size();
		a.check(diff, circle, collisions);
		for (; j < collisions.size(); ++j)
		{
			ret.insert(std::make_pair(r - collisions[j].pen, &a));
		}
	}

	for (size_t i = 0; i < rectangles.size(); ++i)
	{
		auto& a = rectangles[i];
		if (!a.entity)
			continue;

		if ((a.layers & layers) == 0)
			continue;

		if (filter)
			if (!filter(&a))
				continue;

		Vec2 diff = p - a.entity->xy;

		size_t j = collisions.size();
		a.check(diff, circle, collisions);
		for (; j < collisions.size(); ++j)
		{
			ret.insert(std::make_pair(r - collisions[j].pen, &a));
		}
	}

	for (size_t i = 0; i < tilemaps.size(); ++i)
	{
		auto& a = tilemaps[i];
		if (!a.entity)
			continue;

		if ((a.layers & layers) == 0)
			continue;

		if (filter)
			if (!filter(&a))
				continue;

		Vec2 diff = p - a.entity->xy;

		size_t j = collisions.size();
		a.check(diff, circle, collisions);
		for (; j < collisions.size(); ++j)
		{
			ret.insert(std::make_pair(r - collisions[j].pen, &a));
		}
	}

	return ret;
}
