#include "CustomBehaviourSystem.h"

#include "CustomBehaviour.h"

void CustomBehaviourSystem::tick(float dt)
{
	auto & added = level->added;

	for (auto& behaviour : added)
		behaviour->components.data()->start();
	added.clear();

	auto & behaviours = level->custom_behaviours.components;

	for (size_t i = 0; i < behaviours.size(); ++i)
	{
		auto behaviour = behaviours[i]->components.data();
		if (behaviour)
			if (behaviour->entity)
				behaviour->tick(dt);
	}
}
