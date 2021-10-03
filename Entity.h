#pragma once

#include <cstdint>

#include "Vec2.h"
#include "Vec3.h"
#include "Quaternion.h"
#include "Matrix4.h"

#include "Reference.h"

#include "UID.h"

#include <string>
#include <map>
#include <vector>

class Component;

class Entity
{
public:
	Entity(const Entity& entity);
	~Entity();

	template <class T>
	Reference<T> getComponent() const
	{
		for (auto& component : components)
		{
			auto ref = component.cast<T>();
			if (ref)
				return ref;
		}
		return nullptr;
	}

	const std::vector<Reference<Component>>& getComponents();

	Vec3 getPosition() const;
	Matrix4 getTransform() const;

	Reference<Entity> getParent() const;
	Reference<Entity> getRoot() const;

	static void adopt(const Reference<Entity> & child, const Reference<Entity> & parent);
	static void orphan(const Reference<Entity> & child);

	const std::vector<Reference<Entity>>& getChildren();

	union
	{
		Vec2 xy;
		Vec3 xyz;
		struct
		{
			float x, y, z;
		};
	};
	Quaternion rotation;
	Vec3 scale;

	UID uid;

private:
	friend class ComponentContainer<Entity>;
	friend class Component;
	friend class Level;

	Entity();

	std::vector<Reference<Component>> components;

	void setRoot(const Reference<Entity> & root);

	Reference<Entity> parent, root;
	std::vector<Reference<Entity>> children;
};

