#pragma once

#include <cstdint>

#include "Vec2.h"

#include <string>
#include <map>

class Component;

class Entity
{
public:
	Entity();
	Entity(Entity && entity) noexcept;
	~Entity();

	Entity & operator=(Entity && entity) noexcept;

	template <class T>
	void addComponent(T * component)
	{
		component->entity = this;
		components.insert(std::make_pair(typeid(T).name(), component));
	}

	template <class T>
	T * getComponent()
	{
		auto component = components.find(typeid(T).name());
		if (component != components.end())
			return (T*)component->second;
		return nullptr;
	}

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
};

