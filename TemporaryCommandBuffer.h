#pragma once

#include <vulkan/vulkan.h>

class TemporaryCommandBuffer
{
public:
	TemporaryCommandBuffer(VkDevice device, VkCommandPool command_pool);
	~TemporaryCommandBuffer();

	void submit(VkQueue queue);

	operator VkCommandBuffer() const;

private:
	VkDevice device;
	VkCommandPool command_pool;
	VkCommandBuffer command_buffer;
};

