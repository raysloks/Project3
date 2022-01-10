#include "Component.h"

Component::Component()
{
	entity = nullptr;
}

Component::~Component()
{
}

void Component::attach(const Reference<Component>& component, const Reference<Entity>& entity)
{
	if (component->entity)
		return;

	component->entity = entity;
	entity->components.push_back(component);
}

void Component::detach(const Reference<Component>& component, const Reference<Entity>& entity)
{
	if (component->entity != entity)
		return;

	auto & components = entity->components;
	for (auto i = components.begin(); i != components.end(); ++i)
	{
		if (*i == component)
		{
			(*i)->entity = nullptr;
			components.erase(i);
			return;
		}
	}
}