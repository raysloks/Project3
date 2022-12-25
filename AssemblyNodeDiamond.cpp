#include "AssemblyNodeDiamond.h"
#include <iostream>
#include <sstream>
#include "AssemblyContext.h"
#include "Coal.h"
#include <typeinfo>
#include <typeindex>
#include "FileNode.h"
#include "LayerNode.h"
#include "Model.h"
#include "SpriteSheet.h"
const std::unordered_map<std::string, std::function<void(AssemblyContext&,const Coal&)>> create_funcs = {
	{ "FileNode<SpriteSheet>", [] (AssemblyContext& context, const Coal& coal) {
auto node = new FileNode<SpriteSheet>();
	if (coal.members.find("fname") != coal.members.end())
		context.registerInput(node->fname, coal["fname"]);
	if (coal.members.find("resource") != coal.members.end())
		context.registerOutput(&node->resource, coal["resource"]);
return node;
}},
	{ "FileNode<Model>", [] (AssemblyContext& context, const Coal& coal) {
auto node = new FileNode<Model>();
	if (coal.members.find("fname") != coal.members.end())
		context.registerInput(node->fname, coal["fname"]);
	if (coal.members.find("resource") != coal.members.end())
		context.registerOutput(&node->resource, coal["resource"]);
return node;
}},
	{ "LayerNode", [] (AssemblyContext& context, const Coal& coal) {
auto node = new LayerNode();
	if (coal.members.find("layers") != coal.members.end())
	{
		node->layers.resize(coal["layers"].elements.size());
		for (size_t i = 0; i < coal["layers"].elements.size(); ++i)
			context.registerInput(node->layers[i], coal["layers"].elements[i]);
	}
	if (coal.members.find("result") != coal.members.end())
		context.registerOutput(&node->result, coal["result"]);
return node;
}} };

const std::unordered_map<std::type_index, std::function<Coal(const AssemblyContext&, AssemblyNode*)>> save_funcs = {
	{ std::type_index(typeid(FileNode<SpriteSheet>)), [] (const AssemblyContext& context, AssemblyNode * ptr) {
auto node = dynamic_cast<FileNode<SpriteSheet>*>(ptr);
Coal coal;
coal.type = Coal::Type::Object;
	coal["fname"] = context.getIndex(node->fname);
	coal["resource"] = context.getIndex(node->resource);
return coal;
}},
	{ std::type_index(typeid(FileNode<Model>)), [] (const AssemblyContext& context, AssemblyNode * ptr) {
auto node = dynamic_cast<FileNode<Model>*>(ptr);
Coal coal;
coal.type = Coal::Type::Object;
	coal["fname"] = context.getIndex(node->fname);
	coal["resource"] = context.getIndex(node->resource);
return coal;
}},
	{ std::type_index(typeid(LayerNode)), [] (const AssemblyContext& context, AssemblyNode * ptr) {
auto node = dynamic_cast<LayerNode*>(ptr);
Coal coal;
coal.type = Coal::Type::Object;
	std::vector<Coal> elements;
	for (size_t i = 0; i < node->layers.size(); ++i)
		elements.push_back(context.getIndex(node->layers[i]));
	coal["layers"] = elements;
	coal["result"] = context.getIndex(node->result);
return coal;
}} };

void Diamond::init(AssemblyContext& obj, const Coal& coal)
{
	if (coal.type != Coal::Type::Object)
		return;
	for (auto& [guid, output] : coal["constants"].members)
	{
		;
	}
	for (auto& node : coal["nodes"].elements)
	{
		create_funcs.at(node["_type"])(obj, node);
	}
}

Coal Diamond::data(const AssemblyContext& obj)
{
	Coal coal;
	coal.type = Coal::Type::Object;
	std::vector<Coal> nodes;
	for (auto& node : obj.nodes)
		nodes.push_back(save_funcs.at(std::type_index(typeid(*node)))(obj, node));
	coal["nodes"] = nodes;
	return coal;
}

