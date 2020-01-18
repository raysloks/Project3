#pragma once

#include "Entity.h"

#include "Tilemap.h"

#include "Xoroshiro128Plus.h"

#include "CircleCollider.h"
#include "RectangleCollider.h"
#include "SpriteCollider.h"
#include "TilemapCollider.h"

#include "CustomBehaviour.h"

#include "Sprite.h"

#include "ComponentContainer.h"

class Level
{
public:
	Level();

	Reference<Entity> add_entity();
	void remove_entity(Reference<Entity> entity);

	Tilemap tilemap;

	Xoroshiro128Plus rng;

	ComponentContainer<CircleCollider> circle_colliders;
	ComponentContainer<RectangleCollider> rectangle_colliders;
	ComponentContainer<SpriteCollider> sprite_colliders;
	ComponentContainer<TilemapCollider> tilemap_colliders;
	ComponentContainer<Sprite> sprites, ui_sprites;

	ComponentContainer<ComponentContainer<CustomBehaviour>*> custom_behaviours;
	std::vector<ComponentContainer<CustomBehaviour>*> added;

	template <class T, class... Args>
	Reference<T> add(Args&&... args)
	{
		ComponentContainer<T> * test = new ComponentContainer<T>();
		auto component = test->add(args...);
		custom_behaviours.add((ComponentContainer<CustomBehaviour>*)test);
		added.push_back((ComponentContainer<CustomBehaviour>*)test);
		return component;
	}

private:
	ComponentContainer<Entity> entities;

	void remove_entity_rough(const Reference<Entity>& entity);
};

