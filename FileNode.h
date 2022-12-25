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
	AssemblyInput<std::string> fname;
	AssemblyOutput<std::shared_ptr<T>> resource;

	void assemble();
};

template<class T>
inline void FileNode<T>::assemble()
{
	auto fname_result = fname.get();
	if (fname_result != nullptr)
		resource.result = T::load(*fname_result);
}
