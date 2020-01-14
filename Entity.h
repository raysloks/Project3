#pragma once

#include <cstdint>

#include "Vec2.h"
#include "Vec3.h"

#include "Reference.h"

#include <string>
#include <map>
#include <vector>

class Component;

class Entity
{
public:
	Entity();
	Entity(Entity && entity) noexcept;
	~Entity();

	Entity & operator=(Entity && entity) noexcept;

	void componentMoved(Component * pOld, Component * pNew);

	template <class T>
	void addComponent(T * component)
	{
		component->entity = this;
		components.insert(std::make_pair(typeid(*component).name(), component));
	}

	template <class T>
	void addComponent(Reference<T> component)
	{
		component->entity = this;
		components.insert(std::make_pair(typeid(*component).name(), &*component));
	}

	void removeComponent(Component * component);

	template <class T>
	void removeComponent(T * component)
	{
		component->entity = nullptr;
		components.erase(typeid(*component).name());
	}

	template <class T>
	T * getComponent() const
	{
		auto component = components.find(typeid(T).name());
		if (component != components.end())
			return (T*)component->second;
		return nullptr;
	}

	Vec3 getPosition() const;

	Entity * getParent() const;
	Entity * getRoot() const;

	void addChild(Entity * child);
	void removeChild(Entity * child);

	const std::vector<Entity*>& getChildren();

	union
	{
		Vec2 xy;
		Vec3 xyz;
		struct
		{
			float x, y, z;
		};
	};

private:

	uint64_t guid;

	std::map<std::string, Component*> components;

	void childMoved(Entity * pOld, Entity * pNew);
	void setRoot(Entity * root);

	Entity * parent;
	Entity * root;
	std::vector<Entity*> children;
};

