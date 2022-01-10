#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

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
		std::vector<VkAttachmentReference> color_attachment_references;
		VkAttachmentReference depth_stencil_attachment_reference;

		std::vector<VkAttachmentDescription> attachment_descriptions;
	};

	void setSettings(const Settings& settings);
	Settings getSettings() const;

private:
	ModelRenderSystem * mrs;

	std::shared_ptr<RenderPass> render_pass;

	Settings settings;
};
