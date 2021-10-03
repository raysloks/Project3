#include "ModelRenderer.h"

#include <stdexcept>

#include "ModelRenderSystem.h"
#include "VideoMemoryAllocator.h"

ModelRenderer::ModelRenderer()
{
	uniform_vma = nullptr;
}

ModelRenderer::ModelRenderer(const std::string& model, const std::string& texture, const std::string& animation)
{
	this->model = Model::get(model);
	this->texture = SpriteSheet::get(texture);
	if (animation.size())
		this->animation = Animation::get(animation);
	uniform_vma = nullptr;

	animation_time = 0.0f;
}

ModelRenderer::~ModelRenderer()
{
}

VkCommandBuffer ModelRenderer::getCommandBuffer(ModelRenderSystem * mrs, size_t current_image_index)
{
	if (command_buffers.empty() && model->loaded && texture->loaded && (animation == nullptr || animation->loaded))
	{
		command_buffers.resize(mrs->getDescriptorSetCount());
		mrs->allocateSecondaryCommandBuffers(command_buffers);

		texture->createTexture(mrs);

		createUniformBuffers(mrs);
		createDescriptorSets(mrs);

		dirty.resize(mrs->getDescriptorSetCount(), true);

		size_t vertex_buffer_offset;
		size_t index_buffer_offset;
		model->getBufferOffsets(mrs, vertex_buffer_offset, index_buffer_offset);

		VkCommandBufferInheritanceInfo command_buffer_inheritance_info = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
			nullptr,
			mrs->getRenderPass(),
			0,
			VK_NULL_HANDLE,
			VK_FALSE,
			0,
			0
		};

		VkCommandBufferBeginInfo command_buffer_begin_info = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
			&command_buffer_inheritance_info
		};

		for (size_t i = 0; i < command_buffers.size(); ++i)
		{
			auto& command_buffer = command_buffers[i];

			if (vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info))
				throw std::runtime_error("failed to begin recording command buffer.");

			// TODO recreate when graphics pipeline is changed (works now on this machine, but it's undefined behaviour i think)
			vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mrs->getGraphicsPipeline());

			VkBuffer vertex_buffers[] = { mrs->getVideoMemoryAllocator()->buffer };
			VkDeviceSize offsets[] = { vertex_buffer_offset };
			vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);

			vkCmdBindIndexBuffer(command_buffer, mrs->getVideoMemoryAllocator()->buffer, index_buffer_offset, VK_INDEX_TYPE_UINT16);

			vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mrs->getPipelineLayout(), 0, 1, &descriptor_sets[i], 0, nullptr);

			vkCmdDrawIndexed(command_buffer, (uint32_t)model->triangles.size() * 3, 1, 0, 0, 0);

			if (vkEndCommandBuffer(command_buffer))
				throw std::runtime_error("failed to record command buffer.");
		}
	}

	if (command_buffers.empty())
		return nullptr;

	return command_buffers[current_image_index];
}

void ModelRenderer::updateUniformBuffer(ModelRenderSystem * mrs, size_t current_image_index)
{
	if (uniform_buffer_offsets.empty())
		return;

	if (dirty[current_image_index])
	{
		dirty[current_image_index] = false;

		uniform_buffer_object.model = entity->getTransform();
		uniform_buffer_object.bones[0] = Matrix4();
		if (animation && animation->loaded)
		{
			std::vector<Matrix4> transforms(256);
			transforms[255] = Matrix4();
			Animation::Pose pose = animation->base_pose;
			animation->actions["attack"].getPose(pose, animation->base_pose, animation_time);
			pose.apply(transforms.data());
			for (size_t i = 0; i < animation->base_pose_inverse_transforms.size(); ++i)
			{
				uniform_buffer_object.bones[i] = animation->base_pose_inverse_transforms[i] * transforms[i];
			}
			animation_time += 0.05f;
			animation_time = fmodf(animation_time, 130.0f);
		}

		mrs->stageUniformBufferData(&uniform_buffer_object, getUniformBufferObjectSize(), uniform_buffer_offsets[current_image_index]);
	}
}

void ModelRenderer::setDirty()
{
	for (size_t i = 0; i < dirty.size(); ++i)
		dirty[i] = true;
}

void ModelRenderer::createDescriptorSets(ModelRenderSystem * mrs)
{
	if (descriptor_sets.size() > 0)
		return;

	std::vector<VkDescriptorSetLayout> descriptor_set_layouts(mrs->getDescriptorSetCount(), mrs->getDescriptorSetLayout());

	VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		nullptr,
		mrs->getDescriptorPool(),
		(uint32_t)descriptor_set_layouts.size(),
		descriptor_set_layouts.data()
	};

	descriptor_sets.resize(descriptor_set_layouts.size());
	if (vkAllocateDescriptorSets(mrs->getDevice(), &descriptor_set_allocate_info, descriptor_sets.data()))
		throw std::runtime_error("failed to allocate descriptor sets.");

	VkDeviceSize uniform_buffer_size = getUniformBufferObjectSize();

	for (size_t i = 0; i < descriptor_sets.size(); ++i)
	{
		VkDescriptorBufferInfo ubo_descriptor_buffer_info = {
			uniform_vma->buffer,
			uniform_buffer_offsets[i],
			uniform_buffer_size
		};

		VkDescriptorBufferInfo vp_descriptor_buffer_info = {
			uniform_vma->buffer,
			mrs->getUniformBufferOffset(i),
			sizeof(ModelRenderSystem::UniformBufferObject)
		};

		VkDescriptorImageInfo descriptor_image_info = {
			texture->texture_sampler,
			texture->texture_image_view,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		};

		VkWriteDescriptorSet ubo_write_descriptor_set = {
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			nullptr,
			descriptor_sets[i],
			0,
			0,
			1,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			nullptr,
			&ubo_descriptor_buffer_info,
			nullptr
		};

		VkWriteDescriptorSet vp_write_descriptor_set = {
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			nullptr,
			descriptor_sets[i],
			1,
			0,
			1,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			nullptr,
			&vp_descriptor_buffer_info,
			nullptr
		};

		VkWriteDescriptorSet sampler_write_descriptor_set = {
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			nullptr,
			descriptor_sets[i],
			2,
			0,
			1,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			&descriptor_image_info,
			nullptr,
			nullptr
		};

		std::array<VkWriteDescriptorSet, 3> write_descriptor_sets = {
			ubo_write_descriptor_set,
			vp_write_descriptor_set,
			sampler_write_descriptor_set
		};

		vkUpdateDescriptorSets(mrs->getDevice(), (uint32_t)write_descriptor_sets.size(), write_descriptor_sets.data(), 0, nullptr);
	}
}

void ModelRenderer::createUniformBuffers(ModelRenderSystem * mrs)
{
	if (uniform_vma)
		return;

	uniform_vma = mrs->getUniformBufferVideoMemoryAllocator();

	VkDeviceSize buffer_size = getUniformBufferObjectSize();

	uniform_buffer_offsets.resize(mrs->getDescriptorSetCount());
	for (size_t i = 0; i < uniform_buffer_offsets.size(); ++i)
		uniform_buffer_offsets[i] = uniform_vma->allocate(buffer_size);
}

void ModelRenderer::freeCommandBuffers(ModelRenderSystem * mrs)
{
	if (command_buffers.empty())
		return;
	vkFreeCommandBuffers(mrs->getDevice(), mrs->getCommandPool(), command_buffers.size(), command_buffers.data());
	command_buffers.clear();
}

size_t ModelRenderer::getUniformBufferObjectSize() const
{
	if (animation)
	{
		if (animation->loaded)
		{
			return sizeof(uniform_buffer_object) - sizeof(uniform_buffer_object.bones) + sizeof(Matrix4) * animation->base_pose.bones.size();
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return sizeof(uniform_buffer_object) - sizeof(uniform_buffer_object.bones) + sizeof(Matrix4);
	}
}
