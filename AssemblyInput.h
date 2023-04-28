#pragma once

#include "AssemblyInputBase.h"

#include "AssemblyOutput.h"

template <class T>
class AssemblyInput :
	public AssemblyInputBase
{
public:
	using value_type = T;

	bool matches(AssemblyOutputBase * output) const
	{
		return dynamic_cast<AssemblyOutput<T>*>(output);
	}

	const T * get() const
	{
		if (output != nullptr)
			return &static_cast<AssemblyOutput<T>*>(output)->result;
		return nullptr;
	}
};