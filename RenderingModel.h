#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

#include "DynamicState.h"

class Model;
class SpriteSheet;
class ModelRenderSystem;
class GraphicsPipeline;
class VideoMemoryAllocator;

class RenderingModel
{
public:
	RenderingModel(const std::shared_ptr<Model>& model, const std::vector<std::shared_ptr<SpriteSheet>>& textures, size_t uniform_buffer_size, ModelRenderSystem * mrs, const std::shared_ptr<GraphicsPipeline>& graphics_pipeline, size_t camera_index, const DynamicState& dynamic_state);
	~RenderingModel();

	void stageUniformBufferData(void * source_data, size_t image_index);

	std::shared_ptr<GraphicsPipeline> getGraphicsPipeline() const;

	VkCommandBuffer getCommandBuffer(size_t image_index) const;

	DynamicState getDynamicState() const;

private:
	ModelRenderSystem * mrs;

	std::shared_ptr<GraphicsPipeline> graphics_pipeline;

	std::shared_ptr<Model> model;
	std::vector<std::shared_ptr<SpriteSheet>> textures;

	std::vector<VkCommandBuffer> command_buffers;

	VideoMemoryAllocator * uniform_vma;
	size_t uniform_buffer_size;
	std::vector<size_t> uniform_buffer_offsets;
	std::vector<VkDescriptorSet> descriptor_sets;

	void createUniformBuffers();
	void createDescriptorSets();
	void createCommandBuffers();

	size_t camera_index; // temp

	DynamicState dynamic_state;
};
