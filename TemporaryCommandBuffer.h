#pragma once

#include <vulkan/vulkan.h>

class TemporaryCommandBuffer
{
public:
	TemporaryCommandBuffer();
	TemporaryCommandBuffer(VkDevice device, VkCommandPool command_pool, VkFence fence = VK_NULL_HANDLE);
	~TemporaryCommandBuffer();

	void submit(VkQueue queue);

	operator VkCommandBuffer() const;

private:
	VkDevice device;
	VkCommandPool command_pool;
	VkCommandBuffer command_buffer;
	VkFence fence;
};

