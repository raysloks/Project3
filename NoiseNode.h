#pragma once

#include "AssemblyNode.h"

#include "AssemblyOutput.h"
#include "SpriteSheet.h"

class NoiseNode :
	public AssemblyNode
{
public:
	AssemblyOutput<std::shared_ptr<SpriteSheet>> result;

	uint64_t seed0, seed1;

	uint64_t width, height;

	void assemble();
};

