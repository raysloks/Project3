#include "GraphicsPipeline.h"

#include "Model.h"
#include "ModelRenderSystem.h"

#include "PipelineTemplate.h"
#include "RenderPass.h"

GraphicsPipeline::GraphicsPipeline(const PipelineTemplate& pipeline_template, const std::shared_ptr<RenderPass>& render_pass) : mrs(pipeline_template.getModelRenderSystem()), render_pass(render_pass)
{
	VkViewport viewport = {
		0.0f,
		0.0f,
		(float)mrs->getWidth(),
		(float)mrs->getHeight(),
		0.0f,
		1.0f
	};

	VkRect2D scissor = {
		{ 0, 0 },
		{ mrs->getWidth(), mrs->getHeight() }
	};

	VkPipelineViewportStateCreateInfo viewport_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		nullptr,
		0,
		1,
		&viewport,
		1,
		&scissor
	};

	auto settings = pipeline_template.getSettings();

	VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		nullptr,
		0,
		1,
		&settings.vertex_input_binding_description,
		(uint32_t)settings.vertex_input_attribute_descriptions.size(),
		settings.vertex_input_attribute_descriptions.data()
	};

	VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_FALSE,
		VK_LOGIC_OP_COPY,
		1,
		&settings.color_blend_attachment_state,
		{0.0f, 0.0f, 0.0f, 0.0f}
	};

	VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		settings.descriptor_set_layouts.size(),
		settings.descriptor_set_layouts.data(),
		0,
		nullptr
	};

	if (vkCreatePipelineLayout(mrs->getDevice(), &pipeline_layout_create_info, nullptr, &layout))
		throw std::runtime_error("failed to create pipeline layout.");

	VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {
		VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		nullptr,
		0,
		settings.shader_stages.size(),
		settings.shader_stages.data(),
		&vertex_input_state_create_info,
		&settings.input_assembly_state_create_info,
		nullptr,
		&viewport_state_create_info,
		&settings.rasterization_state_create_info,
		&settings.multisample_state_create_info,
		&settings.depth_stencil_state_create_info,
		&color_blend_state_create_info,
		nullptr,
		layout,
		*render_pass,
		0,
		VK_NULL_HANDLE,
		-1
	};

	if (vkCreateGraphicsPipelines(mrs->getDevice(), VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, nullptr, &pipeline))
		throw std::runtime_error("failed to create graphics pipeline.");
}

GraphicsPipeline::~GraphicsPipeline()
{
	vkDestroyPipeline(mrs->getDevice(), pipeline, nullptr);
	vkDestroyPipelineLayout(mrs->getDevice(), layout, nullptr);
}

GraphicsPipeline::operator VkPipeline() const
{
	return pipeline;
}

GraphicsPipeline::operator VkPipelineLayout() const
{
	return layout;
}

VkRenderPass GraphicsPipeline::getRenderPass() const
{
	return *render_pass;
}
