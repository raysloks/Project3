#pragma once

#include <vector>

class AssemblyInputBase;
class AssemblyOutputBase;
class Coal;
class AssemblyNode;

class AssemblyContext
{
public:
	void registerInput(AssemblyInputBase& input, const Coal& coal);
	void registerOutput(AssemblyOutputBase * output, const Coal& coal);

	void addNode(AssemblyNode * node);

	size_t getIndex(AssemblyInputBase& input) const;
	size_t getIndex(AssemblyOutputBase& output) const;

	void assemble();

	std::vector<AssemblyOutputBase*> outputs;
	std::vector<AssemblyNode*> nodes;
};
