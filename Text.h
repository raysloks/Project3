#pragma once

#include "Resource.h"

class Text :
	public std::string, Resource<Text>
{
public:
	Text(const std::string& fname);
};

