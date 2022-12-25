#pragma once

#include <string>
#include <iostream>

class Coal;

class AssemblyContext;

namespace Diamond
{
	void init(AssemblyContext& obj, const Coal& coal);
	Coal data(const AssemblyContext& obj);
}

