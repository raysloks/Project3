#pragma once

#include "Component.h"

#include "Model.h"
#include "SpriteSheet.h"
#include "Animation.h"

#include "Matrix4.h"
#include "Quaternion.h"

#include <vulkan/vulkan.h>

class ModelRenderSystem;

class ModelRenderer :
	public Component
{
public:
	ModelRenderer();
	ModelRenderer(const std::string& model, const std::string& texture, const std::string& animation = "");
	~ModelRenderer();

	VkCommandBuffer getCommandBuffer(ModelRenderSystem * mrs, size_t current_image_index);

	void updateUniformBuffer(ModelRenderSystem * mrs, size_t current_image_index);

	void setDirty();

	void createDescriptorSets(ModelRenderSystem * mrs);

	void createUniformBuffers(ModelRenderSystem * mrs);

	void freeCommandBuffers(ModelRenderSystem * mrs);

	struct UniformBufferObject
	{
		Matrix4 model;
		Matrix4 bones[256];
	};

	float animation_time;

	UniformBufferObject uniform_buffer_object;

	size_t getUniformBufferObjectSize() const;

private:
	std::shared_ptr<Model> model;
	std::shared_ptr<SpriteSheet> texture;
	std::shared_ptr<Animation> animation;

	std::vector<VkCommandBuffer> command_buffers;

	std::weak_ptr<VkPipeline> pipeline;

	VideoMemoryAllocator * uniform_vma;
	std::vector<size_t> uniform_buffer_offsets;
	std::vector<VkDescriptorSet> descriptor_sets;

	std::vector<bool> dirty;
};

