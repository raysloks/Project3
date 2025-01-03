#include "RenderingModel.h"

#include "ModelRenderSystem.h"
#include "VideoMemoryAllocator.h"
#include "GraphicsPipeline.h"

RenderingModel::RenderingModel(const std::shared_ptr<Model>& model, const std::vector<std::shared_ptr<SpriteSheet>>& textures, size_t uniform_buffer_size, ModelRenderSystem * mrs, const std::shared_ptr<GraphicsPipeline>& graphics_pipeline, size_t camera_index, const DynamicState& dynamic_state) : model(model), textures(textures), uniform_buffer_size(uniform_buffer_size), mrs(mrs), uniform_vma(mrs->getUniformBufferVideoMemoryAllocator()), graphics_pipeline(graphics_pipeline), camera_index(camera_index), dynamic_state(dynamic_state)
{
	createUniformBuffers();
	createDescriptorSets();
	createCommandBuffers();
}

RenderingModel::~RenderingModel()
{
	vkFreeCommandBuffers(mrs->getDevice(), mrs->getCommandPool(), (uint32_t)command_buffers.size(), command_buffers.data());
	vkFreeDescriptorSets(mrs->getDevice(), mrs->getDescriptorPool(), (uint32_t)descriptor_sets.size(), descriptor_sets.data());
	for (auto& uniform_buffer_offset : uniform_buffer_offsets)
		uniform_vma->deallocate(uniform_buffer_size, uniform_buffer_offset);
}

void RenderingModel::stageUniformBufferData(void * source_data, size_t image_index)
{
	mrs->stageUniformBufferData(source_data, uniform_buffer_size, uniform_buffer_offsets[image_index]);
}

std::shared_ptr<GraphicsPipeline> RenderingModel::getGraphicsPipeline() const
{
	return graphics_pipeline;
}

VkCommandBuffer RenderingModel::getCommandBuffer(size_t image_index) const
{
    return command_buffers[image_index];
}

DynamicState RenderingModel::getDynamicState() const
{
	return dynamic_state;
}

void RenderingModel::createUniformBuffers()
{
	uniform_buffer_offsets.resize(mrs->getDescriptorSetCount());
	for (auto& uniform_buffer_offset : uniform_buffer_offsets)
		uniform_buffer_offset = uniform_vma->allocate(uniform_buffer_size);
}

void RenderingModel::createDescriptorSets()
{
	for (auto& texture : textures)
		texture->createTexture(mrs);

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

	for (size_t i = 0; i < descriptor_sets.size(); ++i)
	{
		VkDescriptorBufferInfo ubo_descriptor_buffer_info = {
			uniform_vma->buffer,
			uniform_buffer_offsets[i],
			uniform_buffer_size
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

		VkDescriptorBufferInfo vp_descriptor_buffer_info = {
			uniform_vma->buffer,
			mrs->getUniformBufferOffset(camera_index, i),
			sizeof(ModelRenderSystem::UniformBufferObject)
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

		std::vector<VkWriteDescriptorSet> write_descriptor_sets = {
			ubo_write_descriptor_set,
			vp_write_descriptor_set
		};

		std::vector<VkDescriptorImageInfo> descriptor_image_infos(textures.size());

		for (size_t j = 0; j < textures.size(); ++j)
		{
			auto& texture = textures[j];

			descriptor_image_infos[j] = {
				texture->texture_sampler,
				texture->texture_image_view,
				texture->texture_image_layout
			};

			VkWriteDescriptorSet sampler_write_descriptor_set = {
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				descriptor_sets[i],
				j + 2,
				0,
				1,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				&descriptor_image_infos[j],
				nullptr,
				nullptr
			};

			write_descriptor_sets.emplace_back(sampler_write_descriptor_set);
		}

		vkUpdateDescriptorSets(mrs->getDevice(), (uint32_t)write_descriptor_sets.size(), write_descriptor_sets.data(), 0, nullptr);
	}
}

void RenderingModel::createCommandBuffers()
{
	auto settings = graphics_pipeline->getSettings();

	command_buffers.resize(mrs->getDescriptorSetCount());
	mrs->allocateSecondaryCommandBuffers(command_buffers);

	size_t vertex_buffer_offset;
	size_t index_buffer_offset;
	model->getBufferOffsets(mrs, vertex_buffer_offset, index_buffer_offset);

	VkCommandBufferInheritanceInfo command_buffer_inheritance_info = {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
		nullptr,
		graphics_pipeline->getRenderPass(),
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

	bool scissor = std::find(settings.dynamic_states.begin(), settings.dynamic_states.end(), VK_DYNAMIC_STATE_SCISSOR) != settings.dynamic_states.end();

	for (size_t i = 0; i < command_buffers.size(); ++i)
	{
		auto& command_buffer = command_buffers[i];

		if (vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info))
			throw std::runtime_error("failed to begin recording command buffer.");

		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *graphics_pipeline);

		if (scissor)
			vkCmdSetScissor(command_buffer, 0, 1, &dynamic_state.scissor);

		VkBuffer vertex_buffers[] = { mrs->getVideoMemoryAllocator()->buffer };
		VkDeviceSize offsets[] = { vertex_buffer_offset };
		vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);

		vkCmdBindIndexBuffer(command_buffer, mrs->getVideoMemoryAllocator()->buffer, index_buffer_offset, VK_INDEX_TYPE_UINT16);

		vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *graphics_pipeline, 0, 1, &descriptor_sets[i], 0, nullptr);

		vkCmdDrawIndexed(command_buffer, (uint32_t)model->triangles.size() * 3, 1, 0, 0, 0);

		if (vkEndCommandBuffer(command_buffer))
			throw std::runtime_error("failed to record command buffer.");
	}
}
