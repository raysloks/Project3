#pragma once

#include "Resource.h"

class Text :
	public std::string, Resource<Text>
{
public:
	static std::shared_ptr<Text> load(const std::string& fname);
};

