#include "GraphicsPipeline.h"

#include "Model.h"
#include "ModelRenderSystem.h"

#include "PipelineTemplate.h"
#include "RenderPass.h"

GraphicsPipeline::GraphicsPipeline(const PipelineTemplate& pipeline_template, const std::shared_ptr<RenderPass>& render_pass) : mrs(pipeline_template.getModelRenderSystem()), render_pass(render_pass)
{
	auto vertex_input_binding_description = Model::getBindingDescription();
	auto vertex_input_attribute_descriptions = Model::getAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		nullptr,
		0,
		1,
		&vertex_input_binding_description,
		(uint32_t)vertex_input_attribute_descriptions.size(),
		vertex_input_attribute_descriptions.data()
	};

	VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		VK_FALSE
	};

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

	VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_FALSE,
		VK_FALSE,
		VK_POLYGON_MODE_FILL,
		VK_CULL_MODE_BACK_BIT,
		VK_FRONT_FACE_COUNTER_CLOCKWISE,
		VK_FALSE,
		0.0f,
		0.0f,
		0.0f,
		1.0f
	};

	VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_SAMPLE_COUNT_1_BIT,
		VK_FALSE,
		1.0f,
		nullptr,
		VK_FALSE,
		VK_FALSE
	};

	VkPipelineColorBlendAttachmentState color_blend_attachment_state = {
		VK_FALSE,
		VK_BLEND_FACTOR_ONE,
		VK_BLEND_FACTOR_ZERO,
		VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,
		VK_BLEND_FACTOR_ZERO,
		VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};

	VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_TRUE,
		VK_TRUE,
		VK_COMPARE_OP_LESS,
		VK_FALSE,
		VK_FALSE,
		{},
		{},
		0.0f,
		1.0f
	};

	VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_FALSE,
		VK_LOGIC_OP_COPY,
		1,
		&color_blend_attachment_state,
		{0.0f, 0.0f, 0.0f, 0.0f}
	};

	auto settings = pipeline_template.getSettings();

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
		&input_assembly_state_create_info,
		nullptr,
		&viewport_state_create_info,
		&rasterization_state_create_info,
		&multisample_state_create_info,
		&depth_stencil_state_create_info,
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
