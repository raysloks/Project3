#pragma once

#include "Component.h"

#include <vector>
#include <memory>
#include <functional>

#include "Collision.h"
#include "Shape.h"

#include "Circle.h"
#include "Rectangle.h"

class Collider :
	public Component
{
public:
	Collider();

	std::unique_ptr<Shape> shape;

	uint64_t layers;

	std::vector<std::function<void(const Collision&)>> callbacks;
};

