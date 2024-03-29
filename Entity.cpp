#include "Entity.h"

#include "Component.h"

Entity::Entity() : xyz()
{
	parent = nullptr;
	root = nullptr; // set to reference self shortly after construction
	scale = Vec3(1.0f);
}

Entity::Entity(const Entity& entity)
{
	*this = entity;
}

Entity::~Entity()
{
}

void Entity::setRoot(const Reference<Entity>& pNew)
{
	root = pNew;
	for (auto& child : children)
		child->setRoot(pNew);
}

const std::vector<Reference<Component>>& Entity::getComponents()
{
	return components;
}

Vec3 Entity::getPosition() const
{
	if (parent)
		return xyz * parent->getTransform();
	return xyz;
}

Matrix4 Entity::getTransform() const
{
	Matrix4 transform = Matrix4::Scale(scale) * Matrix4(rotation) * Matrix4::Translation(xyz);
	if (parent)
		return transform * parent->getTransform();
	return transform;
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
		auto& children = child->parent->children;
		children.erase(std::find(children.begin(), children.end(), child));
		child->parent = nullptr;
		child->setRoot(child);
	}
}

const std::vector<Reference<Entity>>& Entity::getChildren()
{
	return children;
}
