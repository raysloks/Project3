#pragma once
#include <string>
#include <typeindex>
class Component;
class Coal;
namespace Diamond
{
void print(Component * component);
std::string getDataOf(Component * component);
std::type_index index(const std::string& type);
Component * create(const std::type_index& index);
void init(Component * component, const Coal& coal);
}
