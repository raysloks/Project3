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
		// pipeline settings
		std::vector<VkDescriptorSetLayout> descriptor_set_layouts;
		std::vector<VkPipelineShaderStageCreateInfo> shader_stages;

		VkVertexInputBindingDescription vertex_input_binding_description;
		std::vector<VkVertexInputAttributeDescription> vertex_input_attribute_descriptions;

		VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info;

		VkPipelineRasterizationStateCreateInfo rasterization_state_create_info;

		VkPipelineMultisampleStateCreateInfo multisample_state_create_info;

		std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachment_states;

		VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info;

		std::vector<VkDynamicState> dynamic_states;


		// secondary command buffer settings
		VkCommandBufferInheritanceInfo command_buffer_inheritance_info;
	};

	void setSettings(const Settings& settings);
	Settings getSettings() const;

private:
	ModelRenderSystem * mrs;

	std::shared_ptr<GraphicsPipeline> graphics_pipeline;

	Settings settings;
};
