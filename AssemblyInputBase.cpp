#include "AssemblyInputBase.h"

bool AssemblyInputBase::connect(AssemblyOutputBase * output)
{
	if (matches(output))
	{
		this->output = output;
		return true;
	}
	return false;
}

bool AssemblyInputBase::isConnectedTo(const AssemblyOutputBase * output) const
{
	return this->output == output;
}
