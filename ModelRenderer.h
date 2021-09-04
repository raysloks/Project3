#pragma once

#include "Component.h"

#include "Model.h"
#include "SpriteSheet.h"

#include "Matrix4.h"
#include "Quaternion.h"

#include <vulkan/vulkan.h>

class ModelRenderSystem;

class ModelRenderer :
	public Component
{
public:
	ModelRenderer();
	ModelRenderer(const std::string& model, const std::string& texture);
	~ModelRenderer();

	VkCommandBuffer getCommandBuffer(ModelRenderSystem * mrs, size_t current_image_index);

	void updateUniformBuffer(ModelRenderSystem * mrs, size_t current_image_index);

	void createDescriptorSets(ModelRenderSystem * mrs);

	void createUniformBuffers(ModelRenderSystem * mrs);

	struct UniformBufferObject
	{
		Matrix4 model;
		Matrix4 view;
		Matrix4 proj;
	};

	UniformBufferObject uniform_buffer_object;

private:
	std::shared_ptr<Model> model;
	std::shared_ptr<SpriteSheet> texture;
	std::vector<VkCommandBuffer> command_buffers;

	std::vector<VkBuffer> uniform_buffers;
	std::vector<VkDeviceMemory> uniform_buffer_memories;
	std::vector<VkDescriptorSet> descriptor_sets;

	// could change these to fixed size arrays for small performance gain
	// very unlikely to ever use more than 4
};

