#include "VideoMemoryAllocator.h"

#include <bit>
#include <stdexcept>
#include <iostream>

VideoMemoryAllocator::VideoMemoryAllocator(VkDevice device, ModelRenderSystem * mrs, VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_properties) : device(device)
{
	size_t block_size = 1ll << (VMA_MIN_BIT_COUNT - 1);
	size_t offset = 0;
	for (size_t row_index = 0; row_index < VMA_MAX_HEIGHT; ++row_index)
	{
		std::cout << "block size: " << (block_size >> 10) << " KB" << std::endl;
		vacancy_offsets_category_sizes[row_index] = VMA_MAX_WIDTH >> row_index;
		for (size_t column_index = 0; column_index < vacancy_offsets_category_sizes[row_index]; ++column_index)
		{
			vacancy_offsets[row_index * VMA_MAX_WIDTH + column_index] = offset;
			offset += block_size;
		}
		block_size <<= VMA_BIT_STRIDE;
	}
	size_t size = offset;

	std::cout << "vma size: " << (size >> 20) << " MB" << std::endl;

	VkBufferCreateInfo buffer_create_info = {
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		nullptr,
		0,
		size,
		usage,
		VK_SHARING_MODE_EXCLUSIVE,
		0,
		nullptr
	};

	if (vkCreateBuffer(device, &buffer_create_info, nullptr, &buffer))
		throw std::runtime_error("failed to create buffer.");

	VkMemoryRequirements memory_requirements;
	vkGetBufferMemoryRequirements(device, buffer, &memory_requirements);

	VkMemoryAllocateInfo memory_allocate_info = {
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		nullptr,
		memory_requirements.size,
		mrs->findMemoryType(memory_requirements.memoryTypeBits, memory_properties)
	};

	if (vkAllocateMemory(device, &memory_allocate_info, nullptr, &buffer_memory))
		throw std::runtime_error("failed to allocate buffer memory.");

	vkBindBufferMemory(device, buffer, buffer_memory, 0);
}

VideoMemoryAllocator::~VideoMemoryAllocator()
{
	vkDestroyBuffer(device, buffer, nullptr);
	vkFreeMemory(device, buffer_memory, nullptr);
}

size_t VideoMemoryAllocator::allocate(size_t size)
{
	int row_index = 64 - std::countl_zero(size) - VMA_MIN_BIT_COUNT;
	row_index /= VMA_BIT_STRIDE;
	if (row_index < 0)
		row_index = 0;
	if (row_index >= VMA_MAX_HEIGHT)
		throw std::runtime_error("unable to allocate enough memory.");

	vacancy_offsets_category_sizes[row_index] -= 1;

	return vacancy_offsets[row_index * VMA_MAX_WIDTH + vacancy_offsets_category_sizes[row_index]];
}

void VideoMemoryAllocator::deallocate(size_t size, size_t offset)
{
	int row_index = 64 - std::countl_zero(size) - VMA_MIN_BIT_COUNT;
	row_index /= VMA_BIT_STRIDE;
	if (row_index < 0)
		row_index = 0;
	if (row_index >= VMA_MAX_HEIGHT)
		throw std::runtime_error("unable to deallocate enough memory. (should not have been able to allocate it to begin with)");

	vacancy_offsets[row_index * VMA_MAX_WIDTH + vacancy_offsets_category_sizes[row_index]] = offset;
	vacancy_offsets_category_sizes[row_index] += 1;
}
