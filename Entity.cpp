#include "Entity.h"

#include "Component.h"

uint64_t global_counter = 0;

uint64_t Entity::getGUID() const
{
	return guid;
}

Entity::Entity() : xyz()
{
	guid = ++global_counter;
	parent = nullptr;
	root = nullptr; // set to reference self shortly after construction
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

void Entity::adopt(const Reference<Entity>& child, const Reference<Entity>& parent)
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

void Entity::orphan(const Reference<Entity>& child)
{
	if (child->parent)
	{
		auto & children = child->parent->children;
		children.erase(std::find(children.begin(), children.end(), child));
		child->parent = nullptr;
		child->setRoot(child);
	}
}

const std::vector<Reference<Entity>> & Entity::getChildren()
{
	return children;
}
