#include "CustomBehaviourSystem.h"

#include "CustomBehaviour.h"

void CustomBehaviourSystem::tick(float dt)
{
	for (auto behaviour : behaviours.components)
		if (behaviour->entity)
			behaviour->tick(dt);
}
