#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include <optional>

class ModelRenderSystem;
class RenderPass;

class RenderPassTemplate
{
public:
	RenderPassTemplate(ModelRenderSystem * mrs);

	ModelRenderSystem * getModelRenderSystem() const;

	std::shared_ptr<RenderPass> getRenderPass() const;

	struct Settings
	{
		std::vector<VkAttachmentReference> input_attachment_references;
		std::vector<VkAttachmentReference> color_attachment_references;
		std::optional<VkAttachmentReference> depth_stencil_attachment_reference;

		std::vector<VkAttachmentDescription> attachment_descriptions;

		VkSubpassDependency subpass_dependency;
	};

	void setSettings(const Settings& settings);
	Settings getSettings() const;

private:
	ModelRenderSystem * mrs;

	std::shared_ptr<RenderPass> render_pass;

	Settings settings;
};
