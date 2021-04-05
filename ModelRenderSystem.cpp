#include "ModelRenderSystem.h"

#include <SDL_vulkan.h>

#include "Text.h"

#include <set>
#include <iostream>
#include <thread>

const std::vector<const char*> validation_layer_names = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> device_extensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

ModelRenderSystem::ModelRenderSystem()
{
	window = nullptr;
	instance = nullptr;
	device = nullptr;
	surface = nullptr;
	swapchain = nullptr;
	render_pass = nullptr;
	pipeline_layout = nullptr;
	graphics_pipeline = nullptr;
	command_pool = nullptr;

	max_frames_in_flight = 2;
	current_frame_index = 0;
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

	createGraphicsPipeline();

	createFramebuffers();

	createCommandPool();

	allocateCommandBuffers();

	recordCommandBuffers();

	createSynchronizationPrimitives();
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

	VkPhysicalDeviceFeatures physical_device_features{};

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

	auto format = formats[0];

	int width, height;
	SDL_Vulkan_GetDrawableSize(window, &width, &height);
	VkExtent2D extent{ width, height };

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
	{
		VkComponentMapping component_mapping = {
			VK_COMPONENT_SWIZZLE_IDENTITY,
			VK_COMPONENT_SWIZZLE_IDENTITY,
			VK_COMPONENT_SWIZZLE_IDENTITY,
			VK_COMPONENT_SWIZZLE_IDENTITY
		};

		VkImageSubresourceRange subresource_range = {
			VK_IMAGE_ASPECT_COLOR_BIT,
			0,
			1,
			0,
			1
		};

		VkImageViewCreateInfo image_view_create_info = {
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			nullptr,
			0,
			swapchain_images[i],
			VK_IMAGE_VIEW_TYPE_2D,
			swapchain_format,
			component_mapping,
			subresource_range
		};

		if (vkCreateImageView(device, &image_view_create_info, nullptr, &swapchain_image_views[i]) != VK_SUCCESS)
			throw std::runtime_error("failed to create image view.");
	}
}

void ModelRenderSystem::createRenderPass()
{
	VkAttachmentDescription color_attachment = {
		0,
		swapchain_format,
		VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};

	VkAttachmentReference color_attachment_reference = {
		0,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkSubpassDescription subpass = {
		0,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		0,
		nullptr,
		1,
		&color_attachment_reference,
		nullptr,
		nullptr,
		0,
		nullptr
	};

	VkSubpassDependency subpass_dependency = {
		VK_SUBPASS_EXTERNAL,
		0,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		0,
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		0
	};

	VkRenderPassCreateInfo render_pass_create_info = {
		VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		nullptr,
		0,
		1,
		&color_attachment,
		1,
		&subpass,
		1,
		&subpass_dependency
	};

	if (vkCreateRenderPass(device, &render_pass_create_info, nullptr, &render_pass))
		throw std::runtime_error("failed to create render pass.");
}

void ModelRenderSystem::createGraphicsPipeline()
{
	auto vert_code = Text::get("shaders/vert.spv");
	auto frag_code = Text::get("shaders/frag.spv");

	while (!(vert_code->loaded && frag_code->loaded))
		std::this_thread::yield();

	vert_shader_module = createShaderModule(*vert_code);
	frag_shader_module = createShaderModule(*frag_code);

	VkPipelineShaderStageCreateInfo vert_shader_stage_create_info = {
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0,
		VK_SHADER_STAGE_VERTEX_BIT,
		vert_shader_module,
		"main",
		nullptr
	};

	VkPipelineShaderStageCreateInfo frag_shader_stage_create_info = {
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0,
		VK_SHADER_STAGE_FRAGMENT_BIT,
		frag_shader_module,
		"main",
		nullptr
	};

	VkPipelineShaderStageCreateInfo shader_stages[] = { vert_shader_stage_create_info, frag_shader_stage_create_info };

	VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		nullptr,
		0,
		0,
		nullptr,
		0,
		nullptr
	};

	VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		VK_FALSE
	};

	VkViewport viewport = {
		0.0f,
		0.0f,
		(float)swapchain_extent.width,
		(float)swapchain_extent.height,
		0.0f,
		1.0f
	};

	VkRect2D scissor = {
		{0, 0},
		swapchain_extent
	};

	VkPipelineViewportStateCreateInfo viewport_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		nullptr,
		0,
		1,
		&viewport,
		1,
		&scissor
	};

	VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_FALSE,
		VK_FALSE,
		VK_POLYGON_MODE_FILL,
		VK_CULL_MODE_BACK_BIT,
		VK_FRONT_FACE_CLOCKWISE,
		VK_FALSE,
		0.0f,
		0.0f,
		0.0f,
		1.0f
	};

	VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {
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

	VkPipelineColorBlendAttachmentState color_blend_attachment_state = {
		VK_FALSE,
		VK_BLEND_FACTOR_ONE,
		VK_BLEND_FACTOR_ZERO,
		VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,
		VK_BLEND_FACTOR_ZERO,
		VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};

	VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_FALSE,
		VK_LOGIC_OP_COPY,
		1,
		&color_blend_attachment_state,
		{0.0f, 0.0f, 0.0f, 0.0f}
	};

	VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		0,
		nullptr,
		0,
		nullptr
	};

	if (vkCreatePipelineLayout(device, &pipeline_layout_create_info, nullptr, &pipeline_layout))
		throw std::runtime_error("failed to create pipeline layout.");

	VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {
		VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		nullptr,
		0,
		2,
		shader_stages,
		&vertex_input_state_create_info,
		&input_assembly_state_create_info,
		nullptr,
		&viewport_state_create_info,
		&rasterization_state_create_info,
		&multisample_state_create_info,
		nullptr,
		&color_blend_state_create_info,
		nullptr,
		pipeline_layout,
		render_pass,
		0,
		VK_NULL_HANDLE,
		-1
	};

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, nullptr, &graphics_pipeline))
		throw std::runtime_error("failed to create graphics pipeline.");
}

void ModelRenderSystem::createFramebuffers()
{
	swapchain_framebuffers.resize(swapchain_image_views.size());

	for (size_t i = 0; i < swapchain_framebuffers.size(); ++i)
	{
		VkImageView attachments[] = {
			swapchain_image_views[i]
		};

		VkFramebufferCreateInfo framebuffer_create_info = {
			VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			nullptr,
			0,
			render_pass,
			1,
			attachments,
			swapchain_extent.width,
			swapchain_extent.height,
			1
		};

		if (vkCreateFramebuffer(device, &framebuffer_create_info, nullptr, &swapchain_framebuffers[i]))
			throw std::runtime_error("failed to create framebuffer.");
	}
}

void ModelRenderSystem::createCommandPool()
{
	VkCommandPoolCreateInfo command_pool_create_info = {
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		nullptr,
		0,
		graphics_queue_family_index
	};

	if (vkCreateCommandPool(device, &command_pool_create_info, nullptr, &command_pool))
		throw std::runtime_error("failed to create command pool.");
}

void ModelRenderSystem::allocateCommandBuffers()
{
	command_buffers.resize(swapchain_framebuffers.size());

	VkCommandBufferAllocateInfo command_buffer_allocate_info = {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		nullptr,
		command_pool,
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		(uint32_t)command_buffers.size()
	};

	if (vkAllocateCommandBuffers(device, &command_buffer_allocate_info, command_buffers.data()))
		throw std::runtime_error("failed to allocate command buffers.");
}

void ModelRenderSystem::recordCommandBuffers()
{
	for (size_t i = 0; i < command_buffers.size(); ++i)
	{
		VkCommandBufferBeginInfo command_buffer_begin_info = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			0,
			nullptr
		};

		if (vkBeginCommandBuffer(command_buffers[i], &command_buffer_begin_info))
			throw std::runtime_error("failed to begin recording command buffer.");

		VkClearValue clear_value = { 0.0f, 0.0f, 0.0f, 1.0f };

		VkRenderPassBeginInfo render_pass_begin_info = {
			VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			nullptr,
			render_pass,
			swapchain_framebuffers[i],
			{{0,0},swapchain_extent},
			1,
			&clear_value
		};

		vkCmdBeginRenderPass(command_buffers[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);

		vkCmdDraw(command_buffers[i], 3, 1, 0, 0);

		vkCmdEndRenderPass(command_buffers[i]);

		if (vkEndCommandBuffer(command_buffers[i]))
			throw std::runtime_error("failed to record command buffer.");
	}
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
	in_flight_fences.resize(max_frames_in_flight);
	images_in_flight.resize(swapchain_images.size(), VK_NULL_HANDLE);

	for (size_t i = 0; i < max_frames_in_flight; ++i)
	{
		if (vkCreateSemaphore(device, &semaphore_create_info, nullptr, &image_available_semaphores[i]) ||
			vkCreateSemaphore(device, &semaphore_create_info, nullptr, &render_finished_semaphores[i]) ||
			vkCreateFence(device, &fence_create_info, nullptr, &in_flight_fences[i]))
			throw std::runtime_error("failed to create synchronization primitives.");
	}
}

void ModelRenderSystem::recreateSwapchain()
{
	vkDeviceWaitIdle(device);

	releaseSwapchain();

	createSwapchain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createFramebuffers();
	allocateCommandBuffers();
	recordCommandBuffers();
}

void ModelRenderSystem::release()
{
	vkDeviceWaitIdle(device);

	releaseSwapchain();

	for (auto fence : in_flight_fences)
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
	for (auto framebuffer : swapchain_framebuffers)
		vkDestroyFramebuffer(device, framebuffer, nullptr);

	vkFreeCommandBuffers(device, command_pool, (uint32_t)command_buffers.size(), command_buffers.data());

	vkDestroyPipeline(device, graphics_pipeline, nullptr);
	vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
	vkDestroyRenderPass(device, render_pass, nullptr);

	for (auto image_view : swapchain_image_views)
		vkDestroyImageView(device, image_view, nullptr);

	vkDestroySwapchainKHR(device, swapchain, nullptr);
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

void ModelRenderSystem::tick(float dt)
{
	uint32_t image_index;
	switch (vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, image_available_semaphores[current_frame_index], VK_NULL_HANDLE, &image_index))
	{
	case VK_SUCCESS:
		break;
	case VK_SUBOPTIMAL_KHR:
		break;
	case VK_ERROR_OUT_OF_DATE_KHR:
		recreateSwapchain();
		return;
	default:
		throw std::runtime_error("failed to acquire next image.");
	}

	if (images_in_flight[image_index] != VK_NULL_HANDLE)
		vkWaitForFences(device, 1, &images_in_flight[image_index], VK_TRUE, UINT64_MAX);
	images_in_flight[image_index] = in_flight_fences[current_frame_index];

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
		&command_buffers[image_index],
		1,
		signal_semaphores
	};

	vkResetFences(device, 1, &in_flight_fences[current_frame_index]);

	if (vkQueueSubmit(graphics_queue, 1, &submit_info, in_flight_fences[current_frame_index]))
		throw std::runtime_error("failed to submit draw command buffer.");

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

	++current_frame_index %= max_frames_in_flight;
}
