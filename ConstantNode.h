#pragma once

#include "AssemblyNode.h"

#include "AssemblyOutput.h"

template <class T>
class ConstantNode :
	public AssemblyNode
{
public:
	using value_type = T;

	AssemblyOutput<T> value;

	void assemble();
};

template<class T>
inline void ConstantNode<T>::assemble()
{
}
