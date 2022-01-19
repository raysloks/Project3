#include "RenderPass.h"

#include "ModelRenderSystem.h"
#include "RenderPassTemplate.h"

RenderPass::RenderPass(const RenderPassTemplate& render_pass_template) : mrs(render_pass_template.getModelRenderSystem())
{
	auto settings = render_pass_template.getSettings();

	VkSubpassDescription subpass = {
		0,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		(uint32_t)settings.input_attachment_references.size(),
		settings.input_attachment_references.data(),
		(uint32_t)settings.color_attachment_references.size(),
		settings.color_attachment_references.data(),
		nullptr,
		settings.depth_stencil_attachment_reference.has_value() ? &settings.depth_stencil_attachment_reference.value() : nullptr,
		0,
		nullptr
	};

	VkRenderPassCreateInfo render_pass_create_info = {
		VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		nullptr,
		0,
		(uint32_t)settings.attachment_descriptions.size(),
		settings.attachment_descriptions.data(),
		1,
		&subpass,
		1,
		&settings.subpass_dependency
	};

	render_pass = nullptr;
	if (vkCreateRenderPass(mrs->getDevice(), &render_pass_create_info, nullptr, &render_pass))
		throw std::runtime_error("failed to create render pass.");
}

RenderPass::~RenderPass()
{
	vkDestroyRenderPass(mrs->getDevice(), render_pass, nullptr);
}

RenderPass::operator VkRenderPass() const
{
	return render_pass;
}
