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
	std::unique_ptr<Shape> shape;

	std::vector<std::weak_ptr<std::function<void(const Collision&)>>> callbacks;
};

