#pragma once

#include "System.h"

#include <SDL.h>
#include <vulkan/vulkan.h>

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
	void createGraphicsPipeline();
	void createFramebuffers();
	void createCommandPool();
	void allocateCommandBuffers();
	void recordCommandBuffers();
	void createSynchronizationPrimitives();

	void recreateSwapchain();

	void release();
	void releaseSwapchain();

	VkShaderModule createShaderModule(const std::string& code);

	void tick(float dt);

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

	uint32_t graphics_queue_family_index;
	uint32_t present_queue_family_index;

	VkQueue graphics_queue;
	VkQueue present_queue;

	uint32_t extension_count;
	std::vector<const char*> extension_names;
};

