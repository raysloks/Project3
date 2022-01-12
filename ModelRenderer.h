#pragma once

#include "Component.h"

#include "Model.h"
#include "SpriteSheet.h"
#include "Animation.h"

#include "Matrix4.h"
#include "Vec4.h"
#include "Quaternion.h"

#include <vulkan/vulkan.h>

class ModelRenderSystem;
class RenderingModel;
class RenderContext;

class ModelRenderer :
	public Component
{
public:
	ModelRenderer();
	ModelRenderer(const std::string& model, const std::string& texture, const std::string& animation = "", size_t camera_index = 0);
	~ModelRenderer();

	std::shared_ptr<RenderingModel> getRenderingModel(const RenderContext& render_context);

	void updateUniformBuffer(const RenderContext& render_context);

	void setDirty();

	struct UniformBufferObject
	{
		Matrix4 model;
		Vec4 color;
		Matrix4 bones[256];
	};

	UniformBufferObject uniform_buffer_object;

	size_t getUniformBufferObjectSize() const;

	Animation::Pose pose;

	std::shared_ptr<Model> model;
	std::shared_ptr<SpriteSheet> texture;
	std::shared_ptr<Animation> animation;

private:
	std::shared_ptr<RenderingModel> rendering_model;

	std::vector<VkCommandBuffer> command_buffers;

	std::weak_ptr<VkPipeline> pipeline;

	VideoMemoryAllocator * uniform_vma;
	std::vector<size_t> uniform_buffer_offsets;
	std::vector<VkDescriptorSet> descriptor_sets;

	std::vector<bool> dirty;

	size_t camera_index;
};

