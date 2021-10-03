#pragma once

#include <vulkan/vulkan.h>

#include "ModelRenderSystem.h"
#include "VideoMemoryAllocatorSettings.h"

class VideoMemoryAllocator
{
public:
	VideoMemoryAllocator(VkDevice device, ModelRenderSystem * mrs, const VideoMemoryAllocatorSettings& settings);
	~VideoMemoryAllocator();

	std::vector<size_t> vacancy_offsets;
	std::vector<size_t> vacancy_offsets_category_sizes;

	size_t allocate(size_t size);
	void deallocate(size_t size, size_t offset);

	VkDevice device;

	VkBuffer buffer;
	VkDeviceMemory buffer_memory;

private:
	VideoMemoryAllocatorSettings settings;
};
