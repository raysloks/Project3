#pragma once

class AssemblyOutputBase;

class AssemblyInputBase
{
public:
	virtual ~AssemblyInputBase() {}

	bool connect(AssemblyOutputBase * output);
	virtual bool matches(AssemblyOutputBase * output) const = 0;

	bool isConnectedTo(const AssemblyOutputBase * output) const;

	AssemblyOutputBase * output;
};
