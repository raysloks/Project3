#pragma once

#include "System.h"

#include <memory>
#include <thread>

#include <SDL.h>
#include <vulkan/vulkan.h>

#include "ModelRenderer.h"

#include "Matrix4.h"

class Model;
class SpriteSheet;

class VideoMemoryAllocator;

class ModelRenderSystem :
	public System
{
public:
	ModelRenderSystem();
	~ModelRenderSystem();

	void init(SDL_Window * window);

	void createVulkanInstance();
	void createWindowSurface();
	void selectPhysicalDevice();
	void createLogicalDevice();
	void createSwapchain();
	void createImageViews();
	void createRenderPass();
	void createDescriptorSetLayout();
	void createGraphicsPipeline();
	void createFramebuffers();
	void createCommandPool();
	void createDepthBuffer();
	void createStagingBuffer();
	void allocateCommandBuffers();
	void recordCommandBuffers();
	void createSynchronizationPrimitives();

	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory);

	void stageBufferData(void * source_data, VkDeviceSize size, VkDeviceSize destination_offset);

	void copyBuffer(VkBuffer source, VkBuffer destination, VkDeviceSize size);
	void copyBuffers(VkBuffer source, VkBuffer destination, const std::vector<VkBufferCopy>& regions);

	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memory_properties, VkImage& image, VkDeviceMemory& image_memory);

	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);

	void createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect, VkImageView& image_view);

	void createModelBuffers();

	void createUniformBuffers();

	void allocateSecondaryCommandBuffer(VkCommandBuffer& command_buffer);
	void allocateSecondaryCommandBuffers(std::vector<VkCommandBuffer>& command_buffers);

	void createTextureImage();
	void createTextureImageView();
	void createTextureSampler();

	void updateUniformBuffer(uint32_t current_image_index);

	void createDescriptorPool();
	void createDescriptorSets();

	void recreateSwapchain();

	void release();
	void releaseSwapchain();

	VkDevice getDevice() const;
	VideoMemoryAllocator * getVideoMemoryAllocator() const;

	VkPhysicalDevice getPhysicalDevice() const;

	VkCommandPool getCommandPool() const;
	VkRenderPass getRenderPass() const;

	VkPipelineLayout getPipelineLayout() const;
	VkPipeline getGraphicsPipeline() const;

	VkDescriptorPool getDescriptorPool() const;

	uint32_t findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties);

	VkShaderModule createShaderModule(const std::string& code);

	size_t getDescriptorSetCount() const;
	VkDescriptorSetLayout getDescriptorSetLayout() const;

	void tick(float dt);

	struct UniformBufferObject
	{
		Matrix4 model;
		Matrix4 view;
		Matrix4 proj;
	};

private:
	SDL_Window * window;
	VkInstance instance;
	VkDevice device;

	VkSurfaceKHR surface;

	VkPhysicalDevice physical_device;

	VkSwapchainKHR swapchain;
	std::vector<VkImage> swapchain_images;
	VkFormat swapchain_format;
	VkExtent2D swapchain_extent;
	std::vector<VkImageView> swapchain_image_views;

	VkShaderModule vert_shader_module;
	VkShaderModule frag_shader_module;

	VkRenderPass render_pass;
	VkDescriptorSetLayout descriptor_set_layout;
	VkPipelineLayout pipeline_layout;

	VkPipeline graphics_pipeline;

	std::vector<VkFramebuffer> swapchain_framebuffers;

	VkCommandPool command_pool;
	std::vector<VkCommandBuffer> command_buffers;

	std::vector<VkSemaphore> image_available_semaphores;
	std::vector<VkSemaphore> render_finished_semaphores;
	std::vector<VkFence> in_flight_fences;
	std::vector<VkFence> images_in_flight;

	size_t max_frames_in_flight;
	size_t current_frame_index;
	uint32_t current_image_index;

	uint32_t graphics_queue_family_index;
	uint32_t present_queue_family_index;

	VkQueue graphics_queue;
	VkQueue present_queue;

	uint32_t extension_count;
	std::vector<const char*> extension_names;

	VkDescriptorPool descriptor_pool;
	std::vector<VkDescriptorSet> descriptor_sets;

	VkImage texture_image;
	VkDeviceMemory texture_image_memory;
	VkImageView texture_image_view;
	VkSampler texture_sampler;

	VkImage depth_image;
	VkDeviceMemory depth_image_memory;
	VkImageView depth_image_view;

	std::vector<VkBuffer> uniform_buffers;
	std::vector<VkDeviceMemory> uniform_buffer_memories;

	std::shared_ptr<Model> model;
	std::shared_ptr<SpriteSheet> sprite_sheet;

	VideoMemoryAllocator * vma;
	size_t staging_offset;
	std::vector<VkBufferCopy> staging_regions;
	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_memory;

	ComponentContainer<ModelRenderer> models;

	std::thread present_thread;

	float time;
};

