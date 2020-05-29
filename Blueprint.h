#pragma once

#include "Resource.h"

#include "Entity.h"
#include "Level.h"

class Blueprint :
	public Resource<Blueprint>
{
public:

	static std::shared_ptr<Blueprint> load(const std::string & fname);

	Reference<Entity> build(Level& level) const;
};

