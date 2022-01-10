#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

class ModelRenderSystem;
class PipelineTemplate;
class RenderPass;

class GraphicsPipeline
{
public:
	GraphicsPipeline(const PipelineTemplate& pipeline_template, const std::shared_ptr<RenderPass>& render_pass);
	~GraphicsPipeline();

	operator VkPipeline() const;
	operator VkPipelineLayout() const;

	VkRenderPass getRenderPass() const;

private:
	ModelRenderSystem * mrs;

	VkPipeline pipeline;
	VkPipelineLayout layout;

	std::shared_ptr<RenderPass> render_pass;
};
