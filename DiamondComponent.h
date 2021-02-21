#pragma once

#include <string>
#include <iostream>
#include <typeindex>

class Component;
class Coal;

namespace Diamond
{
	void print(std::ostream& os, Component * component);
	std::type_index index(const std::string& type);
	std::string name(Component * component);
	Component * create(const std::type_index& index);
	void init(Component * component, const Coal& coal);
	Coal data(Component * component);
}

