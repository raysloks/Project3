#pragma once

#include "AssemblyOutputBase.h"

template <class T>
class AssemblyOutput :
	public AssemblyOutputBase
{
public:
	using value_type = T;

	T result;
};