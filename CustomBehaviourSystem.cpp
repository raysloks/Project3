#include "CustomBehaviourSystem.h"

#include "CustomBehaviour.h"

void CustomBehaviourSystem::tick(float dt)
{
	for (auto behaviour : behaviours.components)
		behaviour->tick(dt);
}
