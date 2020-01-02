#pragma once

#include "Entity.h"

class Component
{
public:
	Component();
	Component(Component && component) noexcept;

	Component & operator=(Component && component) noexcept;

	Entity * entity;
};

