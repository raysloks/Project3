#include "ModelRenderSystem.h"

#include <SDL_vulkan.h>

#include "Text.h"
#include "Model.h"
#include "SpriteSheet.h"

#include "Level.h"

#include "Quaternion.h"
#include "Matrix3.h"

#include "TemporaryCommandBuffer.h"
#include "VideoMemoryAllocator.h"

#include "RenderContext.h"
#include "RenderPass.h"

#include <set>
#include <iostream>
#include <thread>
#include <latch>

const std::vector<const char*> validation_layer_names = {
	/*"VK_LAYER_KHRONOS_validation"*/
};

const std::vector<const char*> device_extensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

ModelRenderSystem::ModelRenderSystem() : render_pass_template(this), ui_render_pass_template(this), hbao_render_pass_template(this), graphics_pipeline_template(this), ui_graphics_pipeline_template(this), hbao_graphics_pipeline_template(this), thread_pool(1)
{
	window = nullptr;
	instance = nullptr;
	device = nullptr;
	surface = nullptr;
	swapchain = nullptr;
	command_pool = nullptr;

	max_frames_in_flight = 2;
	current_frame_index = 0;

	present_time_unsafe = SDL_GetPerformanceCounter();
	present_time_safe = present_time_unsafe;

	field_of_view = 70.0f;

	vert_shader_module = nullptr;
	frag_shader_module = nullptr;
	frag_ui_shader_module = nullptr;

	vert_fullscreen_quad_shader_module = nullptr;
	frag_hbao_shader_module = nullptr;

	camera_count = 2;

	hbao_quad.reset(new ModelRenderer());
	hbao_quad->model = std::make_shared<Model>();
	hbao_quad->model->vertices.resize(3);
	hbao_quad->model->triangles = { { 0, 1, 2 } };
	hbao_quad->model->loaded = true;
	hbao_quad->texture = std::make_shared<SpriteSheet>();
}

ModelRenderSystem::~ModelRenderSystem()
{
	release();
}

void ModelRenderSystem::init(SDL_Window * window)
{
	this->window = window;

	createVulkanInstance();

	createWindowSurface();

	selectPhysicalDevice();

	createLogicalDevice();

	createSwapchain();

	createImageViews();

	createRenderPass();
	createUIRenderPass();
	createHBAORenderPass();

	createDescriptorSetLayout();

	setupGraphicsPipelineTemplates();
	graphics_pipeline_template.setRenderPass(render_pass_template.getRenderPass());
	ui_graphics_pipeline_template.setRenderPass(ui_render_pass_template.getRenderPass());
	hbao_graphics_pipeline_template.setRenderPass(hbao_render_pass_template.getRenderPass());

	createDepthBuffer();

	createStagingBuffers();

	createFramebuffers();

	createCommandPool();

	VideoMemoryAllocatorSettings vma_settings = {
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		9,
		4,
		4096,
		4
	};
	vma = new VideoMemoryAllocator(device, this, vma_settings);

	VideoMemoryAllocatorSettings uniform_vma_settings = {
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		9,
		4,
		4096 * 3,
		3
	};
	uniform_vma = new VideoMemoryAllocator(device, this, uniform_vma_settings);

	createDescriptorPool();

	createUniformBuffers();

	allocateCommandBuffers();

	createSynchronizationPrimitives();

	ui.reset(new Window());
}

void ModelRenderSystem::createVulkanInstance()
{
	VkApplicationInfo app_info = {
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		"Project3",
		VK_MAKE_VERSION(0, 0, 1),
		"Project3",
		VK_MAKE_VERSION(0, 0, 1),
		VK_API_VERSION_1_1
	};

	SDL_Vulkan_GetInstanceExtensions(window, &extension_count, nullptr);

	extension_names.resize(extension_count);

	SDL_Vulkan_GetInstanceExtensions(window, &extension_count, extension_names.data());

	VkInstanceCreateInfo instance_create_info = {
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		nullptr,
		0,
		&app_info,
		validation_layer_names.size(),
		validation_layer_names.data(),
		extension_count,
		extension_names.data()
	};

	if (vkCreateInstance(&instance_create_info, nullptr, &instance) != VK_SUCCESS)
		throw std::runtime_error("failed to create instance.");
}

void ModelRenderSystem::createWindowSurface()
{
	if (!SDL_Vulkan_CreateSurface(window, instance, &surface))
		throw std::runtime_error("failed to create surface.");
}

void ModelRenderSystem::selectPhysicalDevice()
{
	uint32_t device_count;
	vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
	std::vector<VkPhysicalDevice> devices(device_count);
	vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

	physical_device = devices[0];

	graphics_queue_family_index = -1;
	present_queue_family_index = -1;

	uint32_t queue_family_count;
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);
	std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());

	for (size_t i = 0; i < queue_family_count; ++i)
	{
		const auto& queue_family = queue_families[i];

		if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			if (graphics_queue_family_index == -1)
				graphics_queue_family_index = i;
		}

		VkBool32 present_support = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_support);
		if (present_support)
		{
			if (present_queue_family_index == -1)
				present_queue_family_index = i;
		}
	}
}

void ModelRenderSystem::createLogicalDevice()
{
	float queue_priorities = 1.0f;

	std::set<uint32_t> queue_family_indices = { graphics_queue_family_index, present_queue_family_index };

	std::vector<VkDeviceQueueCreateInfo> device_queue_create_infos;

	for (auto index : queue_family_indices)
	{
		device_queue_create_infos.push_back(
			VkDeviceQueueCreateInfo{
				VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				nullptr,
				0,
				index,
				1,
				&queue_priorities
			}
		);
	}

	VkPhysicalDeviceFeatures physical_device_features = {};
	physical_device_features.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo device_create_info = {
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		nullptr,
		0,
		device_queue_create_infos.size(),
		device_queue_create_infos.data(),
		0,
		nullptr,
		device_extensions.size(),
		device_extensions.data(),
		&physical_device_features
	};

	if (vkCreateDevice(physical_device, &device_create_info, nullptr, &device) != VK_SUCCESS)
		throw std::runtime_error("failed to create logical device.");

	vkGetDeviceQueue(device, graphics_queue_family_index, 0, &graphics_queue);
	vkGetDeviceQueue(device, present_queue_family_index, 0, &present_queue);
}

void ModelRenderSystem::createSwapchain()
{
	VkSurfaceCapabilitiesKHR surface_capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_capabilities);

	uint32_t image_count = surface_capabilities.minImageCount + 1;
	if (image_count > surface_capabilities.maxImageCount && surface_capabilities.maxImageCount > 0)
		image_count = surface_capabilities.maxImageCount;

	uint32_t format_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, nullptr);
	std::vector<VkSurfaceFormatKHR> formats(format_count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, formats.data());

	auto& format = formats[1]; // OMEGA DIRTY TODO FIX ASAP

	int width, height;
	SDL_Vulkan_GetDrawableSize(window, &width, &height);
	width = std::max(surface_capabilities.maxImageExtent.width, std::min(surface_capabilities.minImageExtent.width, (uint32_t)width));
	height = std::max(surface_capabilities.maxImageExtent.height, std::min(surface_capabilities.minImageExtent.height, (uint32_t)height));
	VkExtent2D extent = { width, height };

	VkSwapchainCreateInfoKHR swapchain_create_info = {
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		nullptr,
		0,
		surface,
		image_count,
		format.format,
		format.colorSpace,
		extent,
		1,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_SHARING_MODE_EXCLUSIVE,
		0,
		nullptr,
		surface_capabilities.currentTransform,
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		VK_PRESENT_MODE_FIFO_KHR,
		VK_TRUE,
		VK_NULL_HANDLE
	};

	if (vkCreateSwapchainKHR(device, &swapchain_create_info, nullptr, &swapchain) != VK_SUCCESS)
		throw std::runtime_error("failed to create swapchain.");

	vkGetSwapchainImagesKHR(device, swapchain, &image_count, nullptr);
	swapchain_images.resize(image_count);
	vkGetSwapchainImagesKHR(device, swapchain, &image_count, swapchain_images.data());

	swapchain_format = format.format;
	swapchain_extent = extent;
}

void ModelRenderSystem::createImageViews()
{
	swapchain_image_views.resize(swapchain_images.size());

	for (size_t i = 0; i < swapchain_images.size(); ++i)
		createImageView(swapchain_images[i], swapchain_format, VK_IMAGE_ASPECT_COLOR_BIT, swapchain_image_views[i]);
}

void ModelRenderSystem::createRenderPass()
{
	RenderPassTemplate::Settings settings;

	VkAttachmentDescription color_attachment = {
		0,
		swapchain_format,
		VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkAttachmentDescription depth_attachment = {
		0,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	};

	VkAttachmentReference color_attachment_reference = {
		0,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkAttachmentReference depth_attachment_reference = {
		1,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	VkSubpassDependency subpass_dependency = {
		VK_SUBPASS_EXTERNAL,
		0,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		0,
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		0
	};

	settings.attachment_descriptions = { color_attachment, depth_attachment };
	settings.color_attachment_references = { color_attachment_reference };
	settings.depth_stencil_attachment_reference = depth_attachment_reference;
	settings.subpass_dependency = subpass_dependency;

	render_pass_template.setSettings(settings);
}

void ModelRenderSystem::createUIRenderPass()
{
	RenderPassTemplate::Settings settings;

	VkAttachmentDescription color_attachment = {
		0,
		swapchain_format,
		VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};

	VkAttachmentDescription depth_attachment = {
		0,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	VkAttachmentReference color_attachment_reference = {
		0,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkAttachmentReference depth_attachment_reference = {
		1,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	VkSubpassDependency subpass_dependency = {
		VK_SUBPASS_EXTERNAL,
		0,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		0,
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		0
	};

	settings.attachment_descriptions = { color_attachment, depth_attachment };
	settings.color_attachment_references = { color_attachment_reference };
	settings.depth_stencil_attachment_reference = depth_attachment_reference;
	settings.subpass_dependency = subpass_dependency;

	ui_render_pass_template.setSettings(settings);
}

void ModelRenderSystem::createHBAORenderPass()
{
	RenderPassTemplate::Settings settings;

	VkAttachmentDescription color_attachment = {
		0,
		swapchain_format,
		VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkAttachmentReference color_attachment_reference = {
		0,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkSubpassDependency subpass_dependency = {
		VK_SUBPASS_EXTERNAL,
		0,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		0,
		0
	};

	settings.attachment_descriptions = { color_attachment };
	settings.color_attachment_references = { color_attachment_reference };
	settings.subpass_dependency = subpass_dependency;

	hbao_render_pass_template.setSettings(settings);
}

void ModelRenderSystem::createDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding ubo_layout_binding = {
		0,
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		1,
		VK_SHADER_STAGE_VERTEX_BIT,
		nullptr
	};

	VkDescriptorSetLayoutBinding vp_layout_binding = {
		1,
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		1,
		VK_SHADER_STAGE_VERTEX_BIT,
		nullptr
	};

	VkDescriptorSetLayoutBinding sampler_layout_binding = {
		2,
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		1,
		VK_SHADER_STAGE_FRAGMENT_BIT,
		nullptr
	};

	std::array<VkDescriptorSetLayoutBinding, 3> descriptor_set_layout_bindings = { ubo_layout_binding, vp_layout_binding, sampler_layout_binding };

	VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		(uint32_t)descriptor_set_layout_bindings.size(),
		descriptor_set_layout_bindings.data()
	};

	if (vkCreateDescriptorSetLayout(device, &descriptor_set_layout_create_info, nullptr, &descriptor_set_layout))
		throw std::runtime_error("failed to create descriptor set layout.");
}

void ModelRenderSystem::createFramebuffers()
{
	swapchain_framebuffers.resize(swapchain_image_views.size());
	hbao_framebuffers.resize(swapchain_image_views.size());

	for (size_t i = 0; i < swapchain_framebuffers.size(); ++i)
	{
		{
			std::vector<VkImageView> attachments = {
				swapchain_image_views[i],
				depth_image_view
			};

			VkFramebufferCreateInfo framebuffer_create_info = {
				VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				nullptr,
				0,
				*render_pass_template.getRenderPass(),
				(uint32_t)attachments.size(),
				attachments.data(),
				swapchain_extent.width,
				swapchain_extent.height,
				1
			};

			if (vkCreateFramebuffer(device, &framebuffer_create_info, nullptr, &swapchain_framebuffers[i]))
				throw std::runtime_error("failed to create framebuffer.");
		}
		{
			std::vector<VkImageView> attachments = {
				swapchain_image_views[i]
			};

			VkFramebufferCreateInfo framebuffer_create_info = {
				VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				nullptr,
				0,
				*hbao_render_pass_template.getRenderPass(),
				(uint32_t)attachments.size(),
				attachments.data(),
				swapchain_extent.width,
				swapchain_extent.height,
				1
			};

			if (vkCreateFramebuffer(device, &framebuffer_create_info, nullptr, &hbao_framebuffers[i]))
				throw std::runtime_error("failed to create framebuffer.");
		}
	}
}

void ModelRenderSystem::createCommandPool()
{
	VkCommandPoolCreateInfo command_pool_create_info = {
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		nullptr,
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		graphics_queue_family_index
	};

	if (vkCreateCommandPool(device, &command_pool_create_info, nullptr, &command_pool))
		throw std::runtime_error("failed to create command pool.");
}

void ModelRenderSystem::createDepthBuffer()
{
	createImage(swapchain_extent.width, swapchain_extent.height, VK_FORMAT_D24_UNORM_S8_UINT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depth_image, depth_image_memory);

	createImageView(depth_image, VK_FORMAT_D24_UNORM_S8_UINT, VK_IMAGE_ASPECT_DEPTH_BIT, depth_image_view);

	hbao_quad->texture->texture_image = depth_image;
	hbao_quad->texture->texture_image_memory = depth_image_memory;
	hbao_quad->texture->texture_image_view = depth_image_view;

	VkPhysicalDeviceProperties physical_device_properties;
	vkGetPhysicalDeviceProperties(getPhysicalDevice(), &physical_device_properties);

	VkSamplerCreateInfo sampler_create_info = {
		VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		nullptr,
		0,
		VK_FILTER_LINEAR,
		VK_FILTER_LINEAR,
		VK_SAMPLER_MIPMAP_MODE_LINEAR,
		VK_SAMPLER_ADDRESS_MODE_REPEAT,
		VK_SAMPLER_ADDRESS_MODE_REPEAT,
		VK_SAMPLER_ADDRESS_MODE_REPEAT,
		0.0f,
		VK_TRUE,
		physical_device_properties.limits.maxSamplerAnisotropy,
		VK_FALSE,
		VK_COMPARE_OP_ALWAYS,
		0.0f,
		0.0f,
		VK_BORDER_COLOR_INT_OPAQUE_BLACK,
		VK_FALSE
	};

	if (vkCreateSampler(getDevice(), &sampler_create_info, nullptr, &hbao_quad->texture->texture_sampler))
		throw std::runtime_error("failed to create texture sampler.");

	hbao_quad->texture->texture_image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	hbao_quad->texture->loaded = true;
}

void ModelRenderSystem::createStagingBuffers()
{
	createBuffer(16 * 1024 * 1024 * 3, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);
	createBuffer(16 * 1024 * 1024 * 3, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniform_staging_buffer, uniform_staging_buffer_memory);

	staging_offsets.resize(max_frames_in_flight);
	uniform_staging_offsets.resize(max_frames_in_flight);

	for (size_t i = 0; i < max_frames_in_flight; ++i)
	{
		staging_offsets[i] = 16 * 1024 * 1024 * i;
		uniform_staging_offsets[i] = 16 * 1024 * 1024 * i;
	}
}

void ModelRenderSystem::allocateCommandBuffers()
{
	command_buffers.resize(swapchain_framebuffers.size(), std::vector<VkCommandBuffer>(3));

	VkCommandBufferAllocateInfo command_buffer_allocate_info = {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		nullptr,
		command_pool,
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		(uint32_t)command_buffers[0].size()
	};

	for (size_t i = 0; i < command_buffers.size(); ++i)
		if (vkAllocateCommandBuffers(device, &command_buffer_allocate_info, command_buffers[i].data()))
			throw std::runtime_error("failed to allocate command buffers.");
}

void ModelRenderSystem::createSynchronizationPrimitives()
{
	VkSemaphoreCreateInfo semaphore_create_info = {
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		nullptr,
		0
	};

	VkFenceCreateInfo fence_create_info = {
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		nullptr,
		VK_FENCE_CREATE_SIGNALED_BIT
	};

	image_available_semaphores.resize(max_frames_in_flight);
	render_finished_semaphores.resize(max_frames_in_flight);
	in_flight_fences.resize(max_frames_in_flight, std::vector<VkFence>(3));
	image_to_frame_index.resize(swapchain_images.size(), -1);

	for (size_t i = 0; i < max_frames_in_flight; ++i)
	{
		if (vkCreateSemaphore(device, &semaphore_create_info, nullptr, &image_available_semaphores[i]) ||
			vkCreateSemaphore(device, &semaphore_create_info, nullptr, &render_finished_semaphores[i]))
			throw std::runtime_error("failed to create semaphores.");
		for (size_t j = 0; j < in_flight_fences[i].size(); ++j)
		{
			if (vkCreateFence(device, &fence_create_info, nullptr, &in_flight_fences[i][j]))
				throw std::runtime_error("failed to create fences.");
		}
	}
}

void ModelRenderSystem::setupGraphicsPipelineTemplates()
{
	auto vert_code = Text::get("shaders/shader.vert.spv");
	auto frag_code = Text::get("shaders/shader.frag.spv");
	auto frag_ui_code = Text::get("shaders/shader_ui.frag.spv");

	auto vert_fullscreen_quad_code = Text::get("shaders/fullscreen_quad.vert.spv");
	auto frag_hbao_code = Text::get("shaders/hbao.frag.spv");

	while (!(vert_code->loaded && frag_code->loaded && frag_ui_code->loaded && vert_fullscreen_quad_code->loaded && frag_hbao_code->loaded))
		std::this_thread::yield();

	vert_shader_module = createShaderModule(*vert_code);
	frag_shader_module = createShaderModule(*frag_code);
	frag_ui_shader_module = createShaderModule(*frag_ui_code);
	vert_fullscreen_quad_shader_module = createShaderModule(*vert_fullscreen_quad_code);
	frag_hbao_shader_module = createShaderModule(*frag_hbao_code);

	VkPipelineShaderStageCreateInfo vert_shader_stage_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		nullptr,
		0,
		VK_SHADER_STAGE_VERTEX_BIT,
		vert_shader_module,
		"main",
		nullptr
	};

	VkPipelineShaderStageCreateInfo frag_shader_stage_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		nullptr,
		0,
		VK_SHADER_STAGE_FRAGMENT_BIT,
		frag_shader_module,
		"main",
		nullptr
	};

	VkPipelineShaderStageCreateInfo frag_ui_shader_stage_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		nullptr,
		0,
		VK_SHADER_STAGE_FRAGMENT_BIT,
		frag_ui_shader_module,
		"main",
		nullptr
	};

	VkPipelineShaderStageCreateInfo vert_fullscreen_quad_shader_stage_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		nullptr,
		0,
		VK_SHADER_STAGE_VERTEX_BIT,
		vert_fullscreen_quad_shader_module,
		"main",
		nullptr
	};

	VkPipelineShaderStageCreateInfo frag_hbao_shader_stage_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		nullptr,
		0,
		VK_SHADER_STAGE_FRAGMENT_BIT,
		frag_hbao_shader_module,
		"main",
		nullptr
	};

	PipelineTemplate::Settings pipeline_settings;

	pipeline_settings.vertex_input_binding_description = Model::getBindingDescription();
	pipeline_settings.vertex_input_attribute_descriptions = Model::getAttributeDescriptions();

	pipeline_settings.input_assembly_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		VK_FALSE
	};

	pipeline_settings.rasterization_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_FALSE,
		VK_FALSE,
		VK_POLYGON_MODE_FILL,
		VK_CULL_MODE_BACK_BIT,
		VK_FRONT_FACE_COUNTER_CLOCKWISE,
		VK_FALSE,
		0.0f,
		0.0f,
		0.0f,
		1.0f
	};

	pipeline_settings.multisample_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_SAMPLE_COUNT_1_BIT,
		VK_FALSE,
		1.0f,
		nullptr,
		VK_FALSE,
		VK_FALSE
	};

	pipeline_settings.color_blend_attachment_state = {
		VK_FALSE,
		VK_BLEND_FACTOR_ONE,
		VK_BLEND_FACTOR_ZERO,
		VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,
		VK_BLEND_FACTOR_ZERO,
		VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};

	pipeline_settings.depth_stencil_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_TRUE,
		VK_TRUE,
		VK_COMPARE_OP_LESS_OR_EQUAL,
		VK_FALSE,
		VK_FALSE,
		{},
		{},
		0.0f,
		1.0f
	};

	pipeline_settings.shader_stages = { vert_shader_stage_create_info, frag_shader_stage_create_info };
	pipeline_settings.descriptor_set_layouts = { descriptor_set_layout };

	graphics_pipeline_template.setSettings(pipeline_settings);

	pipeline_settings.shader_stages = { vert_shader_stage_create_info, frag_ui_shader_stage_create_info };

	pipeline_settings.color_blend_attachment_state = {
		VK_TRUE,
		VK_BLEND_FACTOR_SRC_ALPHA,
		VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,
		VK_BLEND_FACTOR_ZERO,
		VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};

	ui_graphics_pipeline_template.setSettings(pipeline_settings);

	pipeline_settings.vertex_input_attribute_descriptions = {};

	pipeline_settings.depth_stencil_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_FALSE,
		VK_FALSE,
		VK_COMPARE_OP_ALWAYS,
		VK_FALSE,
		VK_FALSE,
		{},
		{},
		0.0f,
		1.0f
	};

	pipeline_settings.shader_stages = { vert_fullscreen_quad_shader_stage_create_info, frag_hbao_shader_stage_create_info };

	hbao_graphics_pipeline_template.setSettings(pipeline_settings);
}

void ModelRenderSystem::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memory_properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory)
{
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
		findMemoryType(memory_requirements.memoryTypeBits, memory_properties)
	};

	if (vkAllocateMemory(device, &memory_allocate_info, nullptr, &buffer_memory))
		throw std::runtime_error("failed to allocate buffer memory.");

	vkBindBufferMemory(device, buffer, buffer_memory, 0);
}

void ModelRenderSystem::stageBufferData(void * source_data, VkDeviceSize size, VkDeviceSize destination_offset)
{
	void * intermediate_data;
	vkMapMemory(device, staging_buffer_memory, staging_offsets[current_frame_index], size, 0, &intermediate_data);
	memcpy(intermediate_data, source_data, (size_t)size);
	vkUnmapMemory(device, staging_buffer_memory);

	VkBufferCopy region = {
		staging_offsets[current_frame_index],
		destination_offset,
		size
	};
	staging_regions.push_back(region);
	constexpr size_t alignment = 64;
	staging_offsets[current_frame_index] += size;
	staging_offsets[current_frame_index] = (staging_offsets[current_frame_index] + alignment - 1) / alignment * alignment;
}

void ModelRenderSystem::stageUniformBufferData(void * source_data, VkDeviceSize size, VkDeviceSize destination_offset)
{
	void * intermediate_data;
	vkMapMemory(device, uniform_staging_buffer_memory, uniform_staging_offsets[current_frame_index], size, 0, &intermediate_data);
	memcpy(intermediate_data, source_data, (size_t)size);
	vkUnmapMemory(device, uniform_staging_buffer_memory);

	VkBufferCopy region = {
		uniform_staging_offsets[current_frame_index],
		destination_offset,
		size
	};
	uniform_staging_regions.push_back(region);
	constexpr size_t alignment = 64;
	uniform_staging_offsets[current_frame_index] += size;
	uniform_staging_offsets[current_frame_index] = (uniform_staging_offsets[current_frame_index] + alignment - 1) / alignment * alignment;
}

void ModelRenderSystem::copyBuffer(VkBuffer source, VkBuffer destination, VkDeviceSize size, VkCommandBuffer command_buffer, VkFence fence)
{
	//TemporaryCommandBuffer command_buffer(device, command_pool, fence);

	//VkBufferCopy region = {
	//	0,
	//	0,
	//	size
	//};

	//vkCmdCopyBuffer(command_buffer, source, destination, 1, &region);

	//command_buffer.submit(graphics_queue);
}

void ModelRenderSystem::copyBuffers(VkBuffer source, VkBuffer destination, const std::vector<VkBufferCopy>& regions, VkCommandBuffer command_buffer, VkFence fence)
{
	VkCommandBufferBeginInfo command_buffer_begin_info = {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		nullptr,
		0,
		nullptr
	};

	if (vkResetCommandBuffer(command_buffer, 0))
		throw std::runtime_error("failed to reset command buffer.");

	if (vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info))
		throw std::runtime_error("failed to begin recording command buffer.");

	vkCmdCopyBuffer(command_buffer, source, destination, (uint32_t)regions.size(), regions.data());

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

	vkQueueSubmit(graphics_queue, 1, &submit_info, fence);
}

void ModelRenderSystem::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	TemporaryCommandBuffer command_buffer(device, command_pool);

	VkBufferImageCopy buffer_image_copy = {
		0,
		0,
		0,
		{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
		{ 0, 0, 0 },
		{ width, height, 1 }
	};

	vkCmdCopyBufferToImage(command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &buffer_image_copy);

	command_buffer.submit(graphics_queue);
}

void ModelRenderSystem::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memory_properties, VkImage& image, VkDeviceMemory& image_memory)
{
	VkImageCreateInfo image_create_info = {
		VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		nullptr,
		0,
		VK_IMAGE_TYPE_2D,
		format,
		{ width, height, 1 },
		1,
		1,
		VK_SAMPLE_COUNT_1_BIT,
		tiling,
		usage,
		VK_SHARING_MODE_EXCLUSIVE,
		0,
		nullptr,
		VK_IMAGE_LAYOUT_UNDEFINED
	};

	if (vkCreateImage(device, &image_create_info, nullptr, &image))
		throw std::runtime_error("failed to create image.");

	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(device, image, &memory_requirements);

	VkMemoryAllocateInfo memory_allocate_info = {
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		nullptr,
		memory_requirements.size,
		findMemoryType(memory_requirements.memoryTypeBits, memory_properties)
	};

	if (vkAllocateMemory(device, &memory_allocate_info, nullptr, &image_memory))
		throw std::runtime_error("failed to allocate image memory.");

	vkBindImageMemory(device, image, image_memory, 0);
}

void ModelRenderSystem::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout)
{
	TemporaryCommandBuffer command_buffer(device, command_pool);

	VkImageMemoryBarrier image_memory_barrier = {
		VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		nullptr,
		0,
		0,
		old_layout,
		new_layout,
		VK_QUEUE_FAMILY_IGNORED,
		VK_QUEUE_FAMILY_IGNORED,
		image,
		{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
	};

	VkPipelineStageFlags source_stage_flags;
	VkPipelineStageFlags destination_stage_flags;

	if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		image_memory_barrier.srcAccessMask = 0;
		image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		source_stage_flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destination_stage_flags = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		source_stage_flags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destination_stage_flags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
		throw std::invalid_argument("unsupported image layout transition.");

	vkCmdPipelineBarrier(command_buffer, source_stage_flags, destination_stage_flags, 0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);

	command_buffer.submit(graphics_queue);
}

void ModelRenderSystem::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect, VkImageView& image_view)
{
	VkComponentMapping component_mapping = {
		VK_COMPONENT_SWIZZLE_IDENTITY,
		VK_COMPONENT_SWIZZLE_IDENTITY,
		VK_COMPONENT_SWIZZLE_IDENTITY,
		VK_COMPONENT_SWIZZLE_IDENTITY
	};

	VkImageSubresourceRange subresource_range = {
		aspect,
		0,
		1,
		0,
		1
	};

	VkImageViewCreateInfo image_view_create_info = {
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		nullptr,
		0,
		image,
		VK_IMAGE_VIEW_TYPE_2D,
		format,
		component_mapping,
		subresource_range
	};

	if (vkCreateImageView(device, &image_view_create_info, nullptr, &image_view) != VK_SUCCESS)
		throw std::runtime_error("failed to create image view.");
}

void ModelRenderSystem::createUniformBuffers()
{
	VkDeviceSize buffer_size = sizeof(UniformBufferObject);

	uniform_buffer_offsets.resize(swapchain_images.size() * camera_count);

	for (size_t i = 0; i < swapchain_images.size() * camera_count; ++i)
	{
		uniform_buffer_offsets[i] = uniform_vma->allocate(buffer_size);
	}
}

std::vector<std::shared_ptr<RenderContext>> ModelRenderSystem::recordCommandBuffer(uint32_t image_index)
{
	auto& command_buffer = command_buffers[image_index][0];

	std::latch latch(1);

	thread_pool.push([&]()
		{
			latch.count_down();
		}
	);

	std::shared_ptr<RenderContext> render_context(new RenderContext(this, image_index, render_pass_template.getRenderPass(), graphics_pipeline_template.getGraphicsPipeline()));
	auto& models = level->models.components;
	for (size_t i = 0; i < models.size(); ++i)
	{
		auto& model = models[i];
		if (model.entity)
		{
			auto rendering_model = model.getRenderingModel(*render_context);
			if (rendering_model)
				render_context->addRenderingModel(rendering_model);
		}
		else
		{
			if (model.entity == nullptr)
			{
				model.entity = Reference<Entity>(nullptr, 1); // hacky way to keep track of which components have been removed
				level->models.remove(i);
			}
		}
	}
	render_context->collectCommandBuffers();

	std::shared_ptr<RenderContext> ui_render_context(new RenderContext(this, image_index, ui_render_pass_template.getRenderPass(), ui_graphics_pipeline_template.getGraphicsPipeline()));
	ui->getRenderingModels(*ui_render_context);
	ui_render_context->collectCommandBuffers();

	std::shared_ptr<RenderContext> hbao_render_context(new RenderContext(this, image_index, hbao_render_pass_template.getRenderPass(), hbao_graphics_pipeline_template.getGraphicsPipeline()));
	auto hbao_quad_rendering_model = hbao_quad->getRenderingModel(*hbao_render_context);
	if (hbao_quad_rendering_model)
		hbao_render_context->addRenderingModel(hbao_quad_rendering_model);
	hbao_render_context->collectCommandBuffers();

	latch.wait();

	VkCommandBufferBeginInfo command_buffer_begin_info = {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		nullptr,
		0,
		nullptr
	};

	if (vkResetCommandBuffer(command_buffer, 0))
		throw std::runtime_error("failed to reset command buffer.");

	if (vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info))
		throw std::runtime_error("failed to begin recording command buffer.");

	std::array<VkClearValue, 2> clear_values = {};
	clear_values[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clear_values[1].depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo render_pass_begin_info = {
		VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		nullptr,
		*render_context->getRenderPass(),
		swapchain_framebuffers[image_index],
		{ { 0, 0 }, swapchain_extent },
		(uint32_t)clear_values.size(),
		clear_values.data()
	};

	VkRenderPassBeginInfo hbao_render_pass_begin_info = {
		VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		nullptr,
		*hbao_render_context->getRenderPass(),
		hbao_framebuffers[image_index],
		{ { 0, 0 }, swapchain_extent },
		(uint32_t)clear_values.size(),
		clear_values.data()
	};

	VkRenderPassBeginInfo ui_render_pass_begin_info = {
		VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		nullptr,
		*ui_render_context->getRenderPass(),
		swapchain_framebuffers[image_index],
		{ { 0, 0 }, swapchain_extent },
		(uint32_t)clear_values.size(),
		clear_values.data()
	};

	vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
	render_context->executeCommands(command_buffer);
	vkCmdEndRenderPass(command_buffer);

	vkCmdBeginRenderPass(command_buffer, &hbao_render_pass_begin_info, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
	hbao_render_context->executeCommands(command_buffer);
	vkCmdEndRenderPass(command_buffer);

	vkCmdBeginRenderPass(command_buffer, &ui_render_pass_begin_info, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
	ui_render_context->executeCommands(command_buffer);
	vkCmdEndRenderPass(command_buffer);

	if (vkEndCommandBuffer(command_buffer))
		throw std::runtime_error("failed to record command buffer.");

	return { render_context, hbao_render_context, ui_render_context };
}

void ModelRenderSystem::allocateSecondaryCommandBuffer(VkCommandBuffer& command_buffer)
{
	VkCommandBufferAllocateInfo command_buffer_allocate_info = {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		nullptr,
		command_pool,
		VK_COMMAND_BUFFER_LEVEL_SECONDARY,
		1
	};

	if (vkAllocateCommandBuffers(device, &command_buffer_allocate_info, &command_buffer))
		throw std::runtime_error("failed to allocate secondary command buffer.");
}

void ModelRenderSystem::allocateSecondaryCommandBuffers(std::vector<VkCommandBuffer>& command_buffers)
{
	VkCommandBufferAllocateInfo command_buffer_allocate_info = {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		nullptr,
		command_pool,
		VK_COMMAND_BUFFER_LEVEL_SECONDARY,
		command_buffers.size()
	};

	if (vkAllocateCommandBuffers(device, &command_buffer_allocate_info, command_buffers.data()))
		throw std::runtime_error("failed to allocate secondary command buffers.");
}

void ModelRenderSystem::updateUniformBuffer(uint32_t current_image_index)
{
	// camera 0
	{
		UniformBufferObject uniform_buffer_object;
		uniform_buffer_object.view = getViewMatrix();
		uniform_buffer_object.proj = Matrix4::Perspective(getFieldOfView(), getAspectRatio(), 20.0f, 50.0f);

		//// TODO change to blue noise
		//uniform_buffer_object.proj.data[12] += (rng.next_float() - 0.5f) * 40.0f / getWidth();
		//uniform_buffer_object.proj.data[13] += (rng.next_float() - 0.5f) * 40.0f / getHeight();

		stageUniformBufferData(&uniform_buffer_object, sizeof(uniform_buffer_object), uniform_buffer_offsets[current_image_index]);
	}

	// camera 1
	{
		UniformBufferObject uniform_buffer_object;
		uniform_buffer_object.view = Matrix4();
		uniform_buffer_object.proj = Matrix4::Scale(Vec3(2.0f / getWidth(), 2.0f / getHeight(), 1.0f));

		ui->maxOffset = Vec2(getWidth() / 2.0f, getHeight() / 2.0f);
		ui->minOffset = -ui->maxOffset;

		stageUniformBufferData(&uniform_buffer_object, sizeof(uniform_buffer_object), uniform_buffer_offsets[current_image_index + 1 * swapchain_images.size()]);
	}
}

void ModelRenderSystem::createDescriptorPool()
{
	uint32_t descriptor_pool_size = 4096 * getDescriptorSetCount();

	VkDescriptorPoolSize uniform_buffer_object_descriptor_pool_size = {
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		descriptor_pool_size
	};

	VkDescriptorPoolSize sampler_descriptor_pool_size = {
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		descriptor_pool_size
	};

	std::array<VkDescriptorPoolSize, 2> descriptor_pool_sizes = {
		uniform_buffer_object_descriptor_pool_size,
		sampler_descriptor_pool_size
	};

	VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
		VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		nullptr,
		VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
		descriptor_pool_size,
		(uint32_t)descriptor_pool_sizes.size(),
		descriptor_pool_sizes.data()
	};

	if (vkCreateDescriptorPool(device, &descriptor_pool_create_info, nullptr, &descriptor_pool))
		throw std::runtime_error("failed to create descriptor pool.");
}

void ModelRenderSystem::recreateSwapchain()
{
	std::lock_guard<std::mutex> lock(present_mutex);
	vkDeviceWaitIdle(device);

	releaseSwapchain();

	createSwapchain();
	createImageViews();
	createRenderPass();
	createUIRenderPass();
	createHBAORenderPass();
	graphics_pipeline_template.setRenderPass(render_pass_template.getRenderPass());
	ui_graphics_pipeline_template.setRenderPass(ui_render_pass_template.getRenderPass());
	hbao_graphics_pipeline_template.setRenderPass(hbao_render_pass_template.getRenderPass());

	createDepthBuffer();
	createFramebuffers();
	allocateCommandBuffers();
}

void ModelRenderSystem::release()
{
	vkDeviceWaitIdle(device);

	releaseSwapchain();

	delete vma;

	vkDestroyDescriptorSetLayout(device, descriptor_set_layout, nullptr);

	for (auto& fences : in_flight_fences)
		for (auto fence : fences)
			vkDestroyFence(device, fence, nullptr);

	for (auto semaphore : render_finished_semaphores)
		vkDestroySemaphore(device, semaphore, nullptr);

	for (auto semaphore : image_available_semaphores)
		vkDestroySemaphore(device, semaphore, nullptr);

	vkDestroyCommandPool(device, command_pool, nullptr);

	vkDestroyDevice(device, nullptr);

	vkDestroySurfaceKHR(instance, surface, nullptr);

	vkDestroyInstance(instance, nullptr);
}

void ModelRenderSystem::releaseSwapchain()
{
	vkDestroyImageView(device, depth_image_view, nullptr);
	vkDestroyImage(device, depth_image, nullptr);
	vkFreeMemory(device, depth_image_memory, nullptr);

	for (auto framebuffer : swapchain_framebuffers)
		vkDestroyFramebuffer(device, framebuffer, nullptr);

	for (auto& command_buffers : command_buffers)
		vkFreeCommandBuffers(device, command_pool, (uint32_t)command_buffers.size(), command_buffers.data());

	for (auto image_view : swapchain_image_views)
		vkDestroyImageView(device, image_view, nullptr);

	vkDestroySwapchainKHR(device, swapchain, nullptr);
}

VkDevice ModelRenderSystem::getDevice() const
{
	return device;
}

VideoMemoryAllocator * ModelRenderSystem::getVideoMemoryAllocator() const
{
	return vma;
}

VideoMemoryAllocator * ModelRenderSystem::getUniformBufferVideoMemoryAllocator() const
{
	return uniform_vma;
}

VkPhysicalDevice ModelRenderSystem::getPhysicalDevice() const
{
	return physical_device;
}

VkCommandPool ModelRenderSystem::getCommandPool() const
{
	return command_pool;
}

VkDescriptorPool ModelRenderSystem::getDescriptorPool() const
{
	return descriptor_pool;
}

size_t ModelRenderSystem::getUniformBufferOffset(size_t camera_index, size_t image_index) const
{
	return uniform_buffer_offsets[image_index + camera_index * swapchain_images.size()];
}

uint32_t ModelRenderSystem::findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
	vkGetPhysicalDeviceMemoryProperties(physical_device, &physical_device_memory_properties);

	for (uint32_t i = 0; i < physical_device_memory_properties.memoryTypeCount; ++i)
	{
		if (type_filter & (1 << i) && (physical_device_memory_properties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;
	}

	throw std::runtime_error("failed to find suitable memory type.");
}

VkShaderModule ModelRenderSystem::createShaderModule(const std::string& code)
{
	VkShaderModule shader_module;

	VkShaderModuleCreateInfo shader_module_create_info = {
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0,
		code.size(),
		reinterpret_cast<const uint32_t*>(code.data())
	};

	if (vkCreateShaderModule(device, &shader_module_create_info, nullptr, &shader_module) != VK_SUCCESS)
		throw std::runtime_error("failed to create shader module.");

	return shader_module;
}

size_t ModelRenderSystem::getDescriptorSetCount() const
{
	return swapchain_images.size();
}

VkDescriptorSetLayout ModelRenderSystem::getDescriptorSetLayout() const
{
	return descriptor_set_layout;
}

uint64_t ModelRenderSystem::getPresentTime() const
{
	return present_time_safe;
}

float ModelRenderSystem::getAspectRatio() const
{
	return swapchain_extent.width / (float)swapchain_extent.height;
}

float ModelRenderSystem::getFieldOfView() const
{
	return field_of_view;
}

Matrix4 ModelRenderSystem::getViewMatrix() const
{
	return Matrix4::Translation(-camera_position) * camera_rotation.getConj();
}

uint32_t ModelRenderSystem::getWidth() const
{
	return swapchain_extent.width;
}

uint32_t ModelRenderSystem::getHeight() const
{
	return swapchain_extent.height;
}

Vec2 ModelRenderSystem::screenToWorld(const Vec2& screen_position) const
{
	Matrix4 view_proj = getViewMatrix() * Matrix4::Perspective(field_of_view, getAspectRatio(), 0.1f, 100.0f);

	Matrix3 plane_proj = view_proj;
	plane_proj.mtrx[2][0] = view_proj.mtrx[3][0];
	plane_proj.mtrx[2][1] = view_proj.mtrx[3][1];
	plane_proj.mtrx[2][2] = view_proj.mtrx[3][3];
	plane_proj.mtrx[1][2] = view_proj.mtrx[1][3];
	plane_proj.mtrx[0][2] = view_proj.mtrx[0][3];

	Vec3 pos = plane_proj.Inverse() * Vec3(screen_position.x / swapchain_extent.width * 2.0f - 1.0f, screen_position.y / swapchain_extent.height * 2.0f - 1.0f, 1.0f);

	return pos.xy / pos.z;
}

void ModelRenderSystem::tick(float dt)
{
	auto wait_a_start = SDL_GetPerformanceCounter();
	auto& fences = in_flight_fences[current_frame_index];
	vkWaitForFences(device, fences.size(), fences.data(), VK_TRUE, UINT64_MAX);
	wait_fences_a = SDL_GetPerformanceCounter() - wait_a_start;

	uint32_t image_index;
	switch (vkAcquireNextImageKHR(device, swapchain, 0, image_available_semaphores[current_frame_index], VK_NULL_HANDLE, &image_index))
	{
	case VK_SUCCESS:
		break;
	case VK_SUBOPTIMAL_KHR:
		break;
	case VK_ERROR_OUT_OF_DATE_KHR:
		recreateSwapchain();
		return;
	default:
		return;
	}

	auto wait_b_start = SDL_GetPerformanceCounter();
	if (image_to_frame_index[image_index] != -1 && image_to_frame_index[image_index] != current_frame_index)
	{
		auto& fences = in_flight_fences[image_to_frame_index[image_index]];
		vkWaitForFences(device, fences.size(), fences.data(), VK_TRUE, UINT64_MAX);
	}
	wait_fences_b = SDL_GetPerformanceCounter() - wait_b_start;
	image_to_frame_index[image_index] = current_frame_index;

	auto wait_d_start = SDL_GetPerformanceCounter();

	auto render_contexts = recordCommandBuffer(image_index);

	updateUniformBuffer(image_index);

	// this is horribly hacky rn
	for (auto& model : level->models.components)
		model.updateUniformBuffer(*render_contexts[0]);
	ui->updateUniformBuffers(*render_contexts[1]);

	wait_d = SDL_GetPerformanceCounter() - wait_d_start;

	auto wait_c_start = SDL_GetPerformanceCounter();
	std::lock_guard<std::mutex> lock(present_mutex);
	wait_c = SDL_GetPerformanceCounter() - wait_c_start;

	// only reset fence at index 0 (main render fence)
	vkResetFences(device, 1, &fences[0]);

	TemporaryCommandBuffer tmp_cmdbuf_copy_uniform_buffers;
	if (uniform_staging_regions.size())
	{
		// reset fence at index 1
		vkResetFences(device, 1, &fences[1]);
		copyBuffers(uniform_staging_buffer, uniform_vma->buffer, uniform_staging_regions, command_buffers[image_index][1], fences[1]);
		uniform_staging_regions.clear();
		uniform_staging_offsets[current_frame_index] = 16 * 1024 * 1024 * current_frame_index;
	}

	TemporaryCommandBuffer tmp_cmdbuf_copy_buffers;
	if (staging_regions.size())
	{
		// reset fence at index 2
		vkResetFences(device, 1, &fences[2]);
		copyBuffers(staging_buffer, vma->buffer, staging_regions, command_buffers[image_index][2], fences[2]);
		staging_regions.clear();
		staging_offsets[current_frame_index] = 16 * 1024 * 1024 * current_frame_index;
	}

	VkSemaphore wait_semaphores[] = { image_available_semaphores[current_frame_index] };
	VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSemaphore signal_semaphores[] = { render_finished_semaphores[current_frame_index] };

	VkSubmitInfo submit_info = {
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		nullptr,
		1,
		wait_semaphores,
		wait_stages,
		1,
		&command_buffers[image_index][0],
		1,
		signal_semaphores
	};

	if (vkQueueSubmit(graphics_queue, 1, &submit_info, in_flight_fences[current_frame_index][0]))
		throw std::runtime_error("failed to submit draw command buffer.");

	present_time_safe = present_time_unsafe;

	thread_pool.push([this, signal_semaphores, image_index, render_contexts]()
		{
			std::lock_guard<std::mutex> lock(present_mutex);
			VkSwapchainKHR swapchains[] = { swapchain };

			VkPresentInfoKHR present_info = {
				VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
				nullptr,
				1,
				signal_semaphores,
				1,
				swapchains,
				&image_index,
				nullptr
			};

			vkQueuePresentKHR(present_queue, &present_info);
			present_time_unsafe = SDL_GetPerformanceCounter();
		}
	);

	++current_frame_index %= max_frames_in_flight;
}
