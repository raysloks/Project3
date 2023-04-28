#pragma once

#include "AssemblyNode.h"

#include "AssemblyInput.h"

template <class... Types>
class OutputNode;

template <class T>
class OutputNode<T> :
	public AssemblyNode
{
public:
	void assemble() {}

	AssemblyInput<T> input;
};

#include "Model.h"
#include "SpriteSheet.h"

template <>
class OutputNode<std::shared_ptr<Model>, std::shared_ptr<SpriteSheet>> :
	public AssemblyNode
{
public:
	void assemble() {}

	AssemblyInput<std::shared_ptr<Model>> model;
	AssemblyInput<std::shared_ptr<SpriteSheet>> texture;
};
