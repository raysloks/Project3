#include "ModelRenderer.h"

#include <stdexcept>

#include "ModelRenderSystem.h"
#include "VideoMemoryAllocator.h"

ModelRenderer::ModelRenderer()
{
}

ModelRenderer::ModelRenderer(const std::string& model, const std::string& texture)
{
	this->model = Model::get(model);
	this->texture = SpriteSheet::get(texture);
}

ModelRenderer::~ModelRenderer()
{
}

VkCommandBuffer ModelRenderer::getCommandBuffer(ModelRenderSystem * mrs, size_t current_image_index)
{
	if (command_buffers.empty() && model->loaded && texture->loaded)
	{
		command_buffers.resize(mrs->getDescriptorSetCount());
		mrs->allocateSecondaryCommandBuffers(command_buffers);

		texture->createTextureImage(mrs);
		texture->createTextureImageView(mrs);
		texture->createTextureSampler(mrs);

		createUniformBuffers(mrs);
		createDescriptorSets(mrs);

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

	return command_buffers[current_image_index];
}

int flip = 0;

void ModelRenderer::updateUniformBuffer(ModelRenderSystem * mrs, size_t current_image_index)
{
	flip = (flip + 1) % 240;
	uniform_buffer_object.model = Quaternion(flip * M_PI / 240 * 2, Vec3(0.0f, 0.0f, 1.0f));
	uniform_buffer_object.view = Matrix4::Translation(0.0f, 15.0f, -15.0f) * Quaternion(-M_PI * 1.25f, Vec3(1.0f, 0.0f, 0.0f));
	uniform_buffer_object.proj = Matrix4::Perspective(35.0f, 640 / (float)480, 0.1f, 100.0f);

	void * data;
	vkMapMemory(mrs->getDevice(), uniform_buffer_memories[current_image_index], 0, sizeof(uniform_buffer_object), 0, &data);
	memcpy(data, &uniform_buffer_object, sizeof(uniform_buffer_object));
	vkUnmapMemory(mrs->getDevice(), uniform_buffer_memories[current_image_index]);
}

void ModelRenderer::createDescriptorSets(ModelRenderSystem * mrs)
{
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
		VkDescriptorBufferInfo descriptor_buffer_info = {
			uniform_buffers[i],
			0,
			sizeof(UniformBufferObject)
		};

		VkDescriptorImageInfo descriptor_image_info = {
			texture->texture_sampler,
			texture->texture_image_view,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		};

		VkWriteDescriptorSet uniform_buffer_object_write_descriptor_set = {
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			nullptr,
			descriptor_sets[i],
			0,
			0,
			1,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			nullptr,
			&descriptor_buffer_info,
			nullptr
		};

		VkWriteDescriptorSet sampler_write_descriptor_set = {
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			nullptr,
			descriptor_sets[i],
			1,
			0,
			1,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			&descriptor_image_info,
			nullptr,
			nullptr
		};

		std::array<VkWriteDescriptorSet, 2> write_descriptor_sets = {
			uniform_buffer_object_write_descriptor_set,
			sampler_write_descriptor_set
		};

		vkUpdateDescriptorSets(mrs->getDevice(), (uint32_t)write_descriptor_sets.size(), write_descriptor_sets.data(), 0, nullptr);
	}
}

void ModelRenderer::createUniformBuffers(ModelRenderSystem * mrs)
{
	VkDeviceSize buffer_size = sizeof(UniformBufferObject);

	uniform_buffers.resize(mrs->getDescriptorSetCount());
	uniform_buffer_memories.resize(mrs->getDescriptorSetCount());

	for (size_t i = 0; i < mrs->getDescriptorSetCount(); ++i)
	{
		mrs->createBuffer(buffer_size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			uniform_buffers[i],
			uniform_buffer_memories[i]);
	}
}
