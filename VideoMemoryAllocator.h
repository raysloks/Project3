#pragma once

#include <vulkan/vulkan.h>

#include "ModelRenderSystem.h"

constexpr int VMA_MIN_BIT_COUNT = 9;
constexpr int VMA_BIT_STRIDE = 4;
constexpr size_t VMA_MAX_WIDTH = 4096;
constexpr size_t VMA_MAX_HEIGHT = 4;

class VideoMemoryAllocator
{
public:
	VideoMemoryAllocator(VkDevice device, ModelRenderSystem * mrs, VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_properties);
	~VideoMemoryAllocator();

	size_t vacancy_offsets[VMA_MAX_WIDTH * VMA_MAX_HEIGHT];
	size_t vacancy_offsets_category_sizes[VMA_MAX_HEIGHT];

	size_t allocate(size_t size);
	void deallocate(size_t size, size_t offset);

	VkDevice device;

	VkBuffer buffer;
	VkDeviceMemory buffer_memory;
};
