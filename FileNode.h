#pragma once

#include "AssemblyNode.h"

#include <string>
#include <memory>

#include "AssemblyInput.h"
#include "AssemblyOutput.h"

template <class T>
class FileNode :
	public AssemblyNode
{
public:
	using value_type = T;

	std::string fname;
	AssemblyOutput<std::shared_ptr<T>> resource;

	void assemble();
};

template<class T>
inline void FileNode<T>::assemble()
{
	resource.result = T::get(fname);
}
