#pragma once

#include "Resource.h"

#include <vector>

#include "Vec2.h"

class Model;

class Polygon :
	public Resource<Polygon>
{
public:
	std::shared_ptr<Model> makeSweep(intmax_t steps) const;

	std::vector<Vec2> vertices;
};

