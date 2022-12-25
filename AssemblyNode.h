#pragma once

class AssemblyNode
{
public:
	AssemblyNode();
	virtual ~AssemblyNode() {}

	virtual void assemble() = 0;
};
