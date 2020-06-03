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
	Reference<Entity> get_entity(size_t index);
	void remove_entity(Reference<Entity> entity);

	Reference<Entity> move_entity(Reference<Entity> entity, Level * destination);

	bool contains_entity(Reference<Entity> entity) const;

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

	template <class T>
	Reference<T> get()
	{
		for (size_t i = 0; i < custom_behaviours.components.size(); ++i)
		{
			auto & container = custom_behaviours.components[i];
			if (container)
			{
				auto behaviour = dynamic_cast<T*>(container->components.data());
				if (behaviour && behaviour->entity)
					return Reference<T>((ComponentContainer<T>*)container, 0);
			}
		}
		return nullptr;
	}

	ComponentContainer<Entity> entities;

	void remove_entity_rough(const Reference<Entity>& entity);
};

