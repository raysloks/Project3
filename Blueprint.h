#pragma once

#include "Resource.h"

#include "Entity.h"
#include "Level.h"

class Blueprint :
	public Resource<Blueprint>
{
public:
	Blueprint();
	Blueprint(Reference<Entity> entity);

	static std::shared_ptr<Blueprint> load(const std::string & fname);

	void save(const std::string & fname) const;

	Reference<Entity> build(Level& level) const;

	Level level;
};

