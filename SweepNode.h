#pragma once

#include "AssemblyNode.h"

#include "AssemblyInput.h"
#include "AssemblyOutput.h"

#include "Polygon.h"
#include "Model.h"

class SweepNode :
	public AssemblyNode
{
public:
	AssemblyInput<std::shared_ptr<Polygon>> outline;
	AssemblyOutput<std::shared_ptr<Model>> result;

	uint64_t steps;

	void assemble();
};

