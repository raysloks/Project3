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
		auto & container = behaviours[i];
		if (container)
		{
			auto behaviour = container->components.data();
			if (behaviour->entity)
				behaviour->tick(dt);
			else
			{
				level->custom_behaviours.remove(i);
				delete container;
				container = nullptr;
			}
		}
	}
}
