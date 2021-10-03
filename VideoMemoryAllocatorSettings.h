#pragma once

#include <vulkan/vulkan.h>

class VideoMemoryAllocatorSettings
{
public:
	VkBufferUsageFlags usage;
	VkMemoryPropertyFlags memory_properties;
	int min_bit_count;
	int bit_stride;
	size_t max_width;
	size_t max_height;
};
