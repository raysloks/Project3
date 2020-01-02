#include "Entity.h"

#include "Component.h"

uint64_t global_counter = 0;

Entity::Entity() : p()
{
	guid = ++global_counter;
	parent = nullptr;
	root = this;
}

Entity::Entity(Entity && entity) noexcept
{
	guid = entity.guid;
	entity.guid = 0;

	p = entity.p;

	// reconnect new components
	components = std::move(entity.components);
	for (auto component : components)
		component.second->entity = this;

	// reconnect new parent
	parent = entity.parent;
	entity.parent = nullptr;
	if (parent)
		parent->childMoved(&entity, this);

	// reconnect new children
	children = std::move(entity.children);
	for (auto child : children)
		child->parent = this;

	// update roots
	root = entity.root;
	if (root = &entity)
		setRoot(this);
	entity.root = &entity;

	// the order of these operations could be improved in terms of computational efficiency
}

Entity::~Entity()
{
	for (auto component : components)
		component.second->entity = nullptr;
	for (auto child : children)
	{
		child->parent = nullptr;
		child->setRoot(child);
	}
	if (parent)
		parent->removeChild(this);
}

Entity & Entity::operator=(Entity && entity) noexcept
{
	guid = entity.guid;
	entity.guid = 0;

	p = entity.p;

	// disconnect current components
	for (auto component : components)
		component.second->entity = nullptr;

	// disconnect from current parent
	if (parent)
		parent->removeChild(this);

	// disconnect current children
	for (auto child : children)
	{
		child->parent = nullptr;
		child->setRoot(child);
	}

	// reconnect new components
	components = std::move(entity.components);
	for (auto component : components)
		component.second->entity = this;

	// reconnect new parent
	parent = entity.parent;
	entity.parent = nullptr;
	if (parent)
		parent->childMoved(&entity, this);

	// reconnect new children
	children = std::move(entity.children);
	for (auto child : children)
		child->parent = this;

	// update roots
	root = entity.root;
	if (root = &entity)
		setRoot(this);
	entity.root = &entity;

	// the order of these operations could be improved in terms of computational efficiency

	return *this;
}

void Entity::componentMoved(Component * pOld, Component * pNew)
{
	for (auto& i : components)
	{
		if (i.second == pOld)
		{
			i.second = pNew;
			return;
		}
	}
}

void Entity::childMoved(Entity * pOld, Entity * pNew)
{
	for (auto& child : children)
	{
		if (child == pOld)
		{
			child = pNew;
			return;
		}
	}
}

void Entity::setRoot(Entity * pNew)
{
	root = pNew;
	for (auto child : children)
		child->setRoot(pNew);
}

void Entity::removeComponent(Component * component)
{
	for (auto& i : components)
	{
		if (i.second == component)
		{
			i.second->entity = nullptr;
			components.erase(i.first);
			return;
		}
	}
}

Vec2 Entity::getPosition()
{
	if (parent)
		return p + parent->getPosition();
	return p;
}

Entity * Entity::getParent()
{
	return parent;
}

Entity * Entity::getRoot()
{
	return root;
}

void Entity::addChild(Entity * child)
{
	if (child->root == root)
		return;
	if (child->parent)
		child->parent->removeChild(child);
	child->parent = this;
	child->setRoot(root);
	children.push_back(child);
}

void Entity::removeChild(Entity * child)
{
	children.erase(std::find(children.begin(), children.end(), child));
	child->parent = nullptr;
	child->setRoot(child);
}
