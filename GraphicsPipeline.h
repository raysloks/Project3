#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

#include "PipelineTemplate.h"

class ModelRenderSystem;
class RenderPass;

class GraphicsPipeline
{
public:
	GraphicsPipeline(const PipelineTemplate& pipeline_template, const std::shared_ptr<RenderPass>& render_pass);
	~GraphicsPipeline();

	operator VkPipeline() const;
	operator VkPipelineLayout() const;

	VkRenderPass getRenderPass() const;

	PipelineTemplate::Settings getSettings() const;

private:
	ModelRenderSystem * mrs;

	VkPipeline pipeline;
	VkPipelineLayout layout;

	std::shared_ptr<RenderPass> render_pass;

	PipelineTemplate::Settings settings;
};
