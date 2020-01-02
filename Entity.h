#pragma once

#include <cstdint>

#include "Vec2.h"

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
	void childMoved(Entity * pOld, Entity * pNew);
	void setRoot(Entity * root);

	template <class T>
	void addComponent(T * component)
	{
		component->entity = this;
		components.insert(std::make_pair(typeid(*component).name(), component));
	}

	void removeComponent(Component * component);

	template <class T>
	void removeComponent(T * component)
	{
		component->entity = nullptr;
		components.erase(typeid(*component).name());
	}

	template <class T>
	T * getComponent()
	{
		auto component = components.find(typeid(T).name());
		if (component != components.end())
			return (T*)component->second;
		return nullptr;
	}

	Vec2 getPosition();

	Entity * getParent();
	Entity * getRoot();

	void addChild(Entity * child);
	void removeChild(Entity * child);

	uint64_t guid;

	union
	{
		Vec2 p;
		struct
		{
			float x, y;
		};
	};

	std::map<std::string, Component*> components;

private:
	Entity * parent;
	Entity * root;
	std::vector<Entity*> children;
};

