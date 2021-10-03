#include "TemporaryCommandBuffer.h"

#include <stdexcept>

TemporaryCommandBuffer::TemporaryCommandBuffer() : device(nullptr), command_pool(nullptr), fence(nullptr), command_buffer(nullptr)
{
}

TemporaryCommandBuffer::TemporaryCommandBuffer(VkDevice device, VkCommandPool command_pool, VkFence fence) : device(device), command_pool(command_pool), fence(fence)
{
	VkCommandBufferAllocateInfo command_buffer_allocate_info = {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		nullptr,
		command_pool,
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		1
	};

	vkAllocateCommandBuffers(device, &command_buffer_allocate_info, &command_buffer);

	VkCommandBufferBeginInfo command_buffer_begin_info = {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		nullptr,
		VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		nullptr
	};

	if (vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info))
		throw std::runtime_error("failed to begin recording temporary command buffer.");
}

TemporaryCommandBuffer::~TemporaryCommandBuffer()
{
	if (command_buffer)
		vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
}

void TemporaryCommandBuffer::submit(VkQueue queue)
{
	if (vkEndCommandBuffer(command_buffer))
		throw std::runtime_error("failed to record command buffer.");

	VkSubmitInfo submit_info = {
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		nullptr,
		0,
		nullptr,
		nullptr,
		1,
		&command_buffer,
		0,
		nullptr
	};

	vkQueueSubmit(queue, 1, &submit_info, fence);
	vkQueueWaitIdle(queue);
}

TemporaryCommandBuffer::operator VkCommandBuffer() const
{
	return command_buffer;
}
