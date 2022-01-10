#pragma once

#include <vulkan/vulkan.h>

class ModelRenderSystem;
class RenderPassTemplate;

class RenderPass
{
public:
	RenderPass(const RenderPassTemplate& render_pass_template);
	~RenderPass();

	operator VkRenderPass() const;

private:
	ModelRenderSystem * mrs;

	VkRenderPass render_pass;
};
