#include "AssemblyNodeDiamond.h"
#include <iostream>
#include <sstream>
#include "AssemblyContext.h"
#include "Coal.h"
#include <typeinfo>
#include <typeindex>
#include "Diamond.h"
#include "ConstantNode.h"
#include "FileNode.h"
#include "LayerNode.h"
#include "Model.h"
#include "NoiseNode.h"
#include "OutputNode.h"
#include "SpriteSheet.h"
#include "SweepNode.h"
#include <memory>
#include <string>
const std::unordered_map<std::string, std::function<void(AssemblyContext&,const Coal&)>> create_funcs = {
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
context.nodes.push_back(node);
}},
	{ "FileNode<SpriteSheet>", [] (AssemblyContext& context, const Coal& coal) {
auto node = new FileNode<SpriteSheet>();
	if (coal.members.find("fname") != coal.members.end())
		node->fname = coal["fname"];
	if (coal.members.find("resource") != coal.members.end())
		context.registerOutput(&node->resource, coal["resource"]);
context.nodes.push_back(node);
}},
	{ "FileNode<Model>", [] (AssemblyContext& context, const Coal& coal) {
auto node = new FileNode<Model>();
	if (coal.members.find("fname") != coal.members.end())
		node->fname = coal["fname"];
	if (coal.members.find("resource") != coal.members.end())
		context.registerOutput(&node->resource, coal["resource"]);
context.nodes.push_back(node);
}},
	{ "OutputNode<std::shared_ptr<Model>,std::shared_ptr<SpriteSheet>>", [] (AssemblyContext& context, const Coal& coal) {
auto node = new OutputNode<std::shared_ptr<Model>,std::shared_ptr<SpriteSheet>>();
	if (coal.members.find("model") != coal.members.end())
		context.registerInput(node->model, coal["model"]);
	if (coal.members.find("texture") != coal.members.end())
		context.registerInput(node->texture, coal["texture"]);
context.nodes.push_back(node);
}},
	{ "ConstantNode<std::string>", [] (AssemblyContext& context, const Coal& coal) {
auto node = new ConstantNode<std::string>();
	if (coal.members.find("value") != coal.members.end())
		context.registerOutput(&node->value, coal["value"]);
context.nodes.push_back(node);
}},
	{ "SweepNode", [] (AssemblyContext& context, const Coal& coal) {
auto node = new SweepNode();
	if (coal.members.find("outline") != coal.members.end())
		context.registerInput(node->outline, coal["outline"]);
	if (coal.members.find("result") != coal.members.end())
		context.registerOutput(&node->result, coal["result"]);
context.nodes.push_back(node);
}},
	{ "NoiseNode", [] (AssemblyContext& context, const Coal& coal) {
auto node = new NoiseNode();
	if (coal.members.find("result") != coal.members.end())
		context.registerOutput(&node->result, coal["result"]);
context.nodes.push_back(node);
}} };

const std::unordered_map<std::type_index, std::function<Coal(const AssemblyContext&, AssemblyNode*)>> save_funcs = {
	{ std::type_index(typeid(LayerNode)), [] (const AssemblyContext& context, AssemblyNode * ptr) {
auto node = dynamic_cast<LayerNode*>(ptr);
Coal coal;
coal.type = Coal::Type::Object;
coal["_type"] = "LayerNode";
	std::vector<Coal> elements;
	for (size_t i = 0; i < node->layers.size(); ++i)
		elements.push_back(context.getIndex(node->layers[i]));
	coal["layers"] = elements;
	coal["result"] = context.getIndex(node->result);
return coal;
}},
	{ std::type_index(typeid(FileNode<SpriteSheet>)), [] (const AssemblyContext& context, AssemblyNode * ptr) {
auto node = dynamic_cast<FileNode<SpriteSheet>*>(ptr);
Coal coal;
coal.type = Coal::Type::Object;
coal["_type"] = "FileNode<SpriteSheet>";
	coal["fname"] = node->fname;
	coal["resource"] = context.getIndex(node->resource);
return coal;
}},
	{ std::type_index(typeid(FileNode<Model>)), [] (const AssemblyContext& context, AssemblyNode * ptr) {
auto node = dynamic_cast<FileNode<Model>*>(ptr);
Coal coal;
coal.type = Coal::Type::Object;
coal["_type"] = "FileNode<Model>";
	coal["fname"] = node->fname;
	coal["resource"] = context.getIndex(node->resource);
return coal;
}},
	{ std::type_index(typeid(OutputNode<std::shared_ptr<Model>,std::shared_ptr<SpriteSheet>>)), [] (const AssemblyContext& context, AssemblyNode * ptr) {
auto node = dynamic_cast<OutputNode<std::shared_ptr<Model>,std::shared_ptr<SpriteSheet>>*>(ptr);
Coal coal;
coal.type = Coal::Type::Object;
coal["_type"] = "OutputNode<std::shared_ptr<Model>,std::shared_ptr<SpriteSheet>>";
	coal["model"] = context.getIndex(node->model);
	coal["texture"] = context.getIndex(node->texture);
return coal;
}},
	{ std::type_index(typeid(ConstantNode<std::string>)), [] (const AssemblyContext& context, AssemblyNode * ptr) {
auto node = dynamic_cast<ConstantNode<std::string>*>(ptr);
Coal coal;
coal.type = Coal::Type::Object;
coal["_type"] = "ConstantNode<std::string>";
	coal["value"] = context.getIndex(node->value);
return coal;
}},
	{ std::type_index(typeid(SweepNode)), [] (const AssemblyContext& context, AssemblyNode * ptr) {
auto node = dynamic_cast<SweepNode*>(ptr);
Coal coal;
coal.type = Coal::Type::Object;
coal["_type"] = "SweepNode";
	coal["outline"] = context.getIndex(node->outline);
	coal["result"] = context.getIndex(node->result);
return coal;
}},
	{ std::type_index(typeid(NoiseNode)), [] (const AssemblyContext& context, AssemblyNode * ptr) {
auto node = dynamic_cast<NoiseNode*>(ptr);
Coal coal;
coal.type = Coal::Type::Object;
coal["_type"] = "NoiseNode";
	coal["result"] = context.getIndex(node->result);
return coal;
}} };

void Diamond::init(AssemblyContext& obj, const Coal& coal)
{
	if (coal.type != Coal::Type::Object)
		return;
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

