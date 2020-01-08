#include "CustomBehaviourSystem.h"

#include "CustomBehaviour.h"

void CustomBehaviourSystem::tick(float dt)
{
	for (auto& behaviour : added)
		behaviour->start();
	added.clear();

	for (auto& behaviour : behaviours.components)
		if (behaviour)
			if (behaviour->entity)
				behaviour->tick(dt);
			else
				behaviour.reset();
}

CustomBehaviour *  CustomBehaviourSystem::add(const std::shared_ptr<CustomBehaviour> & behaviour)
{
	added.push_back(behaviour);
	behaviours.add(std::shared_ptr<CustomBehaviour>(behaviour));
	return &*behaviour;
}
