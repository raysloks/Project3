#pragma once

#include "AssemblyOutputBase.h"

template <class T>
class AssemblyOutput :
	public AssemblyOutputBase
{
public:
	T result;
};