#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

class Model;
class SpriteSheet;
class ModelRenderSystem;
class GraphicsPipeline;
class VideoMemoryAllocator;

class RenderingModel
{
public:
	RenderingModel(const std::shared_ptr<Model>& model, const std::shared_ptr<SpriteSheet>& texture, size_t uniform_buffer_size, ModelRenderSystem * mrs, const std::shared_ptr<GraphicsPipeline>& graphics_pipeline, size_t camera_index);
	~RenderingModel();

	void stageUniformBufferData(void * source_data, size_t image_index);

	std::shared_ptr<GraphicsPipeline> getGraphicsPipeline() const;

	VkCommandBuffer getCommandBuffer(size_t image_index) const;

private:
	ModelRenderSystem * mrs;

	std::shared_ptr<GraphicsPipeline> graphics_pipeline;

	std::shared_ptr<Model> model;
	std::shared_ptr<SpriteSheet> texture;

	std::vector<VkCommandBuffer> command_buffers;

	VideoMemoryAllocator * uniform_vma;
	size_t uniform_buffer_size;
	std::vector<size_t> uniform_buffer_offsets;
	std::vector<VkDescriptorSet> descriptor_sets;

	void createUniformBuffers();
	void createDescriptorSets();
	void createCommandBuffers();

	size_t camera_index; // temp
};
