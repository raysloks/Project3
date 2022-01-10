#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

class ModelRenderSystem;
class RenderPass;
class RenderPassTemplate;
class GraphicsPipeline;

class PipelineTemplate
{
public:
	PipelineTemplate(ModelRenderSystem * mrs);

	ModelRenderSystem * getModelRenderSystem() const;

	void setRenderPass(const std::shared_ptr<RenderPass>& render_pass);

	std::shared_ptr<GraphicsPipeline> getGraphicsPipeline();

	struct Settings
	{
		std::vector<VkDescriptorSetLayout> descriptor_set_layouts;
		std::vector<VkPipelineShaderStageCreateInfo> shader_stages;
	};

	void setSettings(const Settings& settings);
	Settings getSettings() const;

private:
	ModelRenderSystem * mrs;

	std::shared_ptr<GraphicsPipeline> graphics_pipeline;

	Settings settings;
};
