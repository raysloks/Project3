#include "AssemblyNode.h"

#include "Coal.h"

#include "FileNode.h"
#include "ConstantNode.h"

#include "SpriteSheet.h"
#include "Model.h"
#include "AssemblyContext.h"

void dummy()
{
	FileNode<SpriteSheet> sprite_sheet;
	FileNode<Model> model;

	ConstantNode<std::string> string;
}

void AssemblyContext::registerInput(AssemblyInputBase& input, const Coal& coal)
{
	size_t index = coal;
	if (index < outputs.size())
		input.connect(outputs[coal]);
}

void AssemblyContext::registerOutput(AssemblyOutputBase * output, const Coal& coal)
{
	size_t index = coal;
	if (index == std::numeric_limits<size_t>::max())
		return;
	if (index >= outputs.size())
		outputs.resize(index);
	outputs[index] = output;
}

void AssemblyContext::addNode(AssemblyNode * node)
{
	nodes.push_back(node);
}

size_t AssemblyContext::getIndex(AssemblyInputBase& input) const
{
	for (size_t i = 0; i < outputs.size(); ++i)
		if (input.isConnectedTo(outputs[i]))
			return i;
	return std::numeric_limits<size_t>::max();
}

size_t AssemblyContext::getIndex(AssemblyOutputBase& output) const
{
	for (size_t i = 0; i < outputs.size(); ++i)
		if (&output == outputs[i])
			return i;
	return std::numeric_limits<size_t>::max();
}

void AssemblyContext::assemble()
{
	for (auto& node : nodes)
		node->assemble();
}
