#include "RenderPass.h"

#include "ModelRenderSystem.h"
#include "RenderPassTemplate.h"

RenderPass::RenderPass(const RenderPassTemplate& render_pass_template) : mrs(render_pass_template.getModelRenderSystem())
{
	auto settings = render_pass_template.getSettings();

	VkSubpassDescription subpass = {
		0,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		0,
		nullptr,
		(uint32_t)settings.color_attachment_references.size(),
		settings.color_attachment_references.data(),
		nullptr,
		&settings.depth_stencil_attachment_reference,
		0,
		nullptr
	};

	VkSubpassDependency subpass_dependency = {
		VK_SUBPASS_EXTERNAL,
		0,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		0,
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		0
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
		&subpass_dependency
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
