#include "LayerNode.h"

void LayerNode::assemble()
{
	if (layers.empty())
		return;
	std::vector<std::shared_ptr<SpriteSheet>> results;
	for (auto& layer : layers)
		results.push_back(*layer.get());
	result.result = results[0];
	for (size_t i = 1; i < results.size(); ++i)
		result.result = result.result->merge(results[i]);
}
