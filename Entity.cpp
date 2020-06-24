#include "Entity.h"

#include "Component.h"

Entity::Entity() : xyz()
{
	parent = nullptr;
	root = nullptr; // set to reference self shortly after construction
}

Entity::Entity(const Entity & entity)
{
	*this = entity;
}

Entity::~Entity()
{
}

void Entity::setRoot(const Reference<Entity> & pNew)
{
	root = pNew;
	for (auto child : children)
		child->setRoot(pNew);
}

const std::vector<Reference<Component>>& Entity::getComponents()
{
	return components;
}

Vec3 Entity::getPosition() const
{
	if (parent)
		return xyz + parent->getPosition();
	return xyz;
}

Reference<Entity> Entity::getParent() const
{
	return parent;
}

Reference<Entity> Entity::getRoot() const
{
	return root;
}

void Entity::adopt(const Reference<Entity> & child, const Reference<Entity> & parent)
{
	if (parent->root == child->root)
		return;
	if (child->parent)
		return;
	child->parent = parent;
	auto root = parent->getRoot();
	if (!root)
		root = parent;
	child->setRoot(root);
	parent->children.push_back(child);
}

void Entity::orphan(const Reference<Entity> & child)
{
	if (child->parent)
	{
		auto & children = child->parent->children;
		children.erase(std::find(children.begin(), children.end(), child));
		child->parent = nullptr;
		child->setRoot(child);
	}
}

const std::vector<Reference<Entity>>& Entity::getChildren()
{
	return children;
}
