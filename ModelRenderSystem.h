#pragma once

#include "System.h"

#include <memory>
#include <thread>

#include <SDL.h>
#include <vulkan/vulkan.h>

#include "ModelRenderer.h"

#include "Matrix4.h"

#include "TemporaryCommandBuffer.h"

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
	void createStagingBuffers();
	void allocateCommandBuffers();
	void createSynchronizationPrimitives();

	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory);

	void stageBufferData(void * source_data, VkDeviceSize size, VkDeviceSize destination_offset);
	void stageUniformBufferData(void * source_data, VkDeviceSize size, VkDeviceSize destination_offset);

	void copyBuffer(VkBuffer source, VkBuffer destination, VkDeviceSize size, VkCommandBuffer command_buffer, VkFence fence);
	void copyBuffers(VkBuffer source, VkBuffer destination, const std::vector<VkBufferCopy>& regions, VkCommandBuffer command_buffer, VkFence fence);

	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memory_properties, VkImage& image, VkDeviceMemory& image_memory);

	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);

	void createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect, VkImageView& image_view);

	void createUniformBuffers();

	void recordCommandBuffer(uint32_t image_index);

	void allocateSecondaryCommandBuffer(VkCommandBuffer& command_buffer);
	void allocateSecondaryCommandBuffers(std::vector<VkCommandBuffer>& command_buffers);

	void freeSecondaryCommandBuffers();

	void createTextureImage();
	void createTextureImageView();
	void createTextureSampler();

	void updateUniformBuffer(uint32_t current_image_index);

	void createDescriptorPool();

	void recreateSwapchain();

	void release();
	void releaseSwapchain();

	VkDevice getDevice() const;

	VkPhysicalDevice getPhysicalDevice() const;

	VideoMemoryAllocator * getVideoMemoryAllocator() const;
	VideoMemoryAllocator * getUniformBufferVideoMemoryAllocator() const;

	VkCommandPool getCommandPool() const;
	VkRenderPass getRenderPass() const;

	VkPipelineLayout getPipelineLayout() const;
	VkPipeline getGraphicsPipeline() const;

	VkDescriptorPool getDescriptorPool() const;

	size_t getUniformBufferOffset(size_t image_index) const;

	uint32_t findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties);

	VkShaderModule createShaderModule(const std::string& code);

	size_t getDescriptorSetCount() const;
	VkDescriptorSetLayout getDescriptorSetLayout() const;

	uint64_t getPresentTime() const;

	float getAspectRatio() const;
	float getFieldOfView() const;
	Matrix4 getViewMatrix() const;

	Vec2 screenToWorld(const Vec2& screen_position) const;

	void tick(float dt);

	float field_of_view;

	Vec3 camera_position;
	Quaternion camera_rotation;

	struct UniformBufferObject
	{
		Matrix4 view;
		Matrix4 proj;
	};

	uint64_t wait_fences_a, wait_fences_b;
	uint64_t wait_c, wait_d, wait_e;

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
	std::vector<std::vector<VkCommandBuffer>> command_buffers;

	std::vector<VkSemaphore> image_available_semaphores;
	std::vector<VkSemaphore> render_finished_semaphores;
	std::vector<std::vector<VkFence>> in_flight_fences;
	std::vector<size_t> image_to_frame_index;

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

	VkImage texture_image;
	VkDeviceMemory texture_image_memory;
	VkImageView texture_image_view;
	VkSampler texture_sampler;

	VkImage depth_image;
	VkDeviceMemory depth_image_memory;
	VkImageView depth_image_view;

	std::vector<size_t> uniform_buffer_offsets;

	std::shared_ptr<Model> model;
	std::shared_ptr<SpriteSheet> sprite_sheet;

	VideoMemoryAllocator * vma;
	std::vector<size_t> staging_offsets;
	std::vector<VkBufferCopy> staging_regions;
	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_memory;

	VideoMemoryAllocator * uniform_vma;
	std::vector<size_t> uniform_staging_offsets;
	std::vector<VkBufferCopy> uniform_staging_regions;
	VkBuffer uniform_staging_buffer;
	VkDeviceMemory uniform_staging_buffer_memory;

	std::thread present_thread;

	uint64_t present_time_unsafe, present_time_safe;
};

