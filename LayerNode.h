#pragma once

#include "AssemblyNode.h"

#include <vector>

#include "AssemblyInput.h"
#include "AssemblyOutput.h"
#include "SpriteSheet.h"

class LayerNode :
	public AssemblyNode
{
public:
	std::vector<AssemblyInput<std::shared_ptr<SpriteSheet>>> layers;
	AssemblyOutput<std::shared_ptr<SpriteSheet>> result;

	void assemble();
};
