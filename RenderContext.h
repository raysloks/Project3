#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

#include "DynamicState.h"

class ModelRenderSystem;
class RenderPass;
class GraphicsPipeline;
class RenderingModel;

class RenderContext
{
public:
	RenderContext(ModelRenderSystem * mrs, size_t image_index, const std::shared_ptr<RenderPass>& render_pass, const std::shared_ptr<GraphicsPipeline>& graphics_pipeline);

	ModelRenderSystem * getModelRenderSystem() const;

	size_t getImageIndex() const;
	std::shared_ptr<RenderPass> getRenderPass() const;
	std::shared_ptr<GraphicsPipeline> getGraphicsPipeline() const;

	void addRenderingModel(const std::shared_ptr<RenderingModel>& rendering_model);
	void collectCommandBuffers();
	void executeCommands(VkCommandBuffer command_buffer) const;

	DynamicState dynamic_state;

	void pushDynamicState();
	void popDynamicState();

private:
	ModelRenderSystem * mrs;

	size_t image_index;
	std::shared_ptr<RenderPass> render_pass;
	std::shared_ptr<GraphicsPipeline> graphics_pipeline;

	std::vector<std::shared_ptr<RenderingModel>> rendering_models;
	std::vector<VkCommandBuffer> command_buffers;

	std::vector<DynamicState> dynamic_states;
};

