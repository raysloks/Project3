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

#include <set>
#include <iostream>
#include <thread>

const std::vector<const char*> validation_layer_names = {
	/*"VK_LAYER_KHRONOS_validation"*/
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

	staging_offset = 0;

	time = 0.0f;

	field_of_view = 35.0f;
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

	createDescriptorSetLayout();

	createGraphicsPipeline();

	createDepthBuffer();

	createStagingBuffer();

	createFramebuffers();

	createCommandPool();

	vma = new VideoMemoryAllocator(device, this, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	// temporarily here
	createModelBuffers();
	createTextureImage();
	createTextureImageView();
	createTextureSampler();

	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();

	allocateCommandBuffers();

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

	auto format = formats[0];

	int width, height;
	SDL_Vulkan_GetDrawableSize(window, &width, &height);
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

	VkSubpassDescription subpass = {
		0,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		0,
		nullptr,
		1,
		&color_attachment_reference,
		nullptr,
		&depth_attachment_reference,
		0,
		nullptr
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

	std::array<VkAttachmentDescription, 2> attachments = { color_attachment, depth_attachment };

	VkRenderPassCreateInfo render_pass_create_info = {
		VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		nullptr,
		0,
		(uint32_t)attachments.size(),
		attachments.data(),
		1,
		&subpass,
		1,
		&subpass_dependency
	};

	if (vkCreateRenderPass(device, &render_pass_create_info, nullptr, &render_pass))
		throw std::runtime_error("failed to create render pass.");
}

void ModelRenderSystem::createDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uniform_buffer_object_layout_binding = {
		0,
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		1,
		VK_SHADER_STAGE_VERTEX_BIT,
		nullptr
	};

	VkDescriptorSetLayoutBinding sampler_layout_binding = {
		1,
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		1,
		VK_SHADER_STAGE_FRAGMENT_BIT,
		nullptr
	};

	std::array<VkDescriptorSetLayoutBinding, 2> descriptor_set_layout_bindings = { uniform_buffer_object_layout_binding, sampler_layout_binding };

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

void ModelRenderSystem::createGraphicsPipeline()
{
	auto vert_code = Text::get("shaders/vert.spv");
	auto frag_code = Text::get("shaders/frag.spv");

	while (!(vert_code->loaded && frag_code->loaded))
		std::this_thread::yield();

	vert_shader_module = createShaderModule(*vert_code);
	frag_shader_module = createShaderModule(*frag_code);

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

	VkPipelineShaderStageCreateInfo shader_stages[] = { vert_shader_stage_create_info, frag_shader_stage_create_info };

	auto vertex_input_binding_description = Model::getBindingDescription();
	auto vertex_input_attribute_descriptions = Model::getAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		nullptr,
		0,
		1,
		&vertex_input_binding_description,
		(uint32_t)vertex_input_attribute_descriptions.size(),
		vertex_input_attribute_descriptions.data()
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

	VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info = {
		VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		nullptr,
		0,
		VK_TRUE,
		VK_TRUE,
		VK_COMPARE_OP_LESS,
		VK_FALSE,
		VK_FALSE,
		{},
		{},
		0.0f,
		1.0f
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
		1,
		&descriptor_set_layout,
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
		&depth_stencil_state_create_info,
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
		std::array<VkImageView, 2> attachments = {
			swapchain_image_views[i],
			depth_image_view
		};

		VkFramebufferCreateInfo framebuffer_create_info = {
			VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			nullptr,
			0,
			render_pass,
			(uint32_t)attachments.size(),
			attachments.data(),
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
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		graphics_queue_family_index
	};

	if (vkCreateCommandPool(device, &command_pool_create_info, nullptr, &command_pool))
		throw std::runtime_error("failed to create command pool.");
}

void ModelRenderSystem::createDepthBuffer()
{
	createImage(swapchain_extent.width, swapchain_extent.height, VK_FORMAT_D24_UNORM_S8_UINT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depth_image, depth_image_memory);

	createImageView(depth_image, VK_FORMAT_D24_UNORM_S8_UINT, VK_IMAGE_ASPECT_DEPTH_BIT, depth_image_view);
}

void ModelRenderSystem::createStagingBuffer()
{
	createBuffer(16 * 1024 * 1024, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);
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
	vkMapMemory(device, staging_buffer_memory, staging_offset, size, 0, &intermediate_data);
	memcpy(intermediate_data, source_data, (size_t)size);
	vkUnmapMemory(device, staging_buffer_memory);

	VkBufferCopy region = {
		staging_offset,
		destination_offset,
		size
	};
	staging_regions.push_back(region);
	constexpr size_t alignment = 64;
	staging_offset += size;
	staging_offset = (staging_offset + alignment - 1) / alignment * alignment;
}

void ModelRenderSystem::copyBuffer(VkBuffer source, VkBuffer destination, VkDeviceSize size)
{
	TemporaryCommandBuffer command_buffer(device, command_pool);

	VkBufferCopy region = {
		0,
		0,
		size
	};

	vkCmdCopyBuffer(command_buffer, source, destination, 1, &region);

	command_buffer.submit(graphics_queue);
}

void ModelRenderSystem::copyBuffers(VkBuffer source, VkBuffer destination, const std::vector<VkBufferCopy>& regions)
{
	TemporaryCommandBuffer command_buffer(device, command_pool);

	vkCmdCopyBuffer(command_buffer, source, destination, (uint32_t)regions.size(), regions.data());

	command_buffer.submit(graphics_queue);
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

void ModelRenderSystem::createModelBuffers()
{
	model = Model::get("hoodlum.mdl");

	while (!model->loaded)
		std::this_thread::yield();

	size_t vertex_buffer_offset;
	size_t index_buffer_offset;
	model->getBufferOffsets(this, vertex_buffer_offset, index_buffer_offset);
}

void ModelRenderSystem::createUniformBuffers()
{
	VkDeviceSize buffer_size = sizeof(UniformBufferObject);

	uniform_buffers.resize(swapchain_images.size());
	uniform_buffer_memories.resize(swapchain_images.size());

	for (size_t i = 0; i < swapchain_images.size(); ++i)
	{
		createBuffer(buffer_size, 
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
			uniform_buffers[i], 
			uniform_buffer_memories[i]);
	}
}

void ModelRenderSystem::recordCommandBuffer(uint32_t image_index)
{
	auto& command_buffer = command_buffers[image_index];

	std::vector<VkCommandBuffer> secondary_command_buffers;
	for (auto& model : level->models.components)
		secondary_command_buffers.push_back(model.getCommandBuffer(this, image_index));

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
		render_pass,
		swapchain_framebuffers[image_index],
		{ { 0, 0 }, swapchain_extent },
		(uint32_t)clear_values.size(),
		clear_values.data()
	};

	vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);

	vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

	vkCmdExecuteCommands(command_buffer, secondary_command_buffers.size(), secondary_command_buffers.data());

	vkCmdEndRenderPass(command_buffer);

	if (vkEndCommandBuffer(command_buffer))
		throw std::runtime_error("failed to record command buffer.");
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

void ModelRenderSystem::createTextureImage()
{
	sprite_sheet = SpriteSheet::get("hoodlum.png");

	while (!sprite_sheet->loaded)
		std::this_thread::yield();

	uint32_t width = sprite_sheet->surface->w;
	uint32_t height = sprite_sheet->surface->h;
	VkDeviceSize buffer_size = (VkDeviceSize)width * height * 4;

	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_memory;
	createBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);

	void * data;
	vkMapMemory(device, staging_buffer_memory, 0, buffer_size, 0, &data);
	memcpy(data, sprite_sheet->surface->pixels, (size_t)buffer_size);
	vkUnmapMemory(device, staging_buffer_memory);

	createImage(width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, texture_image, texture_image_memory);

	transitionImageLayout(texture_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	copyBufferToImage(staging_buffer, texture_image, width, height);

	transitionImageLayout(texture_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(device, staging_buffer, nullptr);
	vkFreeMemory(device, staging_buffer_memory, nullptr);
}

void ModelRenderSystem::createTextureImageView()
{
	createImageView(texture_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, texture_image_view);
}

void ModelRenderSystem::createTextureSampler()
{
	VkPhysicalDeviceProperties physical_device_properties;
	vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);

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

	if (vkCreateSampler(device, &sampler_create_info, nullptr, &texture_sampler))
		throw std::runtime_error("failed to create texture sampler.");
}

void ModelRenderSystem::updateUniformBuffer(uint32_t current_image_index)
{
	UniformBufferObject uniform_buffer_object;
	uniform_buffer_object.model = Quaternion(time, Vec3(0.0f, 0.0f, 1.0f));
	uniform_buffer_object.view = Matrix4::Translation(0.0f, 15.0f, -15.0f) * Quaternion(-M_PI * 1.25f, Vec3(1.0f, 0.0f, 0.0f));
	uniform_buffer_object.proj = Matrix4::Perspective(35.0f, swapchain_extent.width / (float)swapchain_extent.height, 0.1f, 100.0f);

	void * data;
	vkMapMemory(device, uniform_buffer_memories[current_image_index], 0, sizeof(uniform_buffer_object), 0, &data);
	memcpy(data, &uniform_buffer_object, sizeof(uniform_buffer_object));
	vkUnmapMemory(device, uniform_buffer_memories[current_image_index]);
}

void ModelRenderSystem::createDescriptorPool()
{
	uint32_t descriptor_pool_size = 1024 * getDescriptorSetCount();

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
		0,
		descriptor_pool_size,
		(uint32_t)descriptor_pool_sizes.size(),
		descriptor_pool_sizes.data()
	};

	if (vkCreateDescriptorPool(device, &descriptor_pool_create_info, nullptr, &descriptor_pool))
		throw std::runtime_error("failed to create descriptor pool.");
}

void ModelRenderSystem::createDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> descriptor_set_layouts(swapchain_images.size(), descriptor_set_layout);

	VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		nullptr,
		descriptor_pool,
		(uint32_t)descriptor_set_layouts.size(),
		descriptor_set_layouts.data()
	};

	descriptor_sets.resize(descriptor_set_layouts.size());
	if (vkAllocateDescriptorSets(device, &descriptor_set_allocate_info, descriptor_sets.data()))
		throw std::runtime_error("failed to allocate descriptor sets.");

	for (size_t i = 0; i < descriptor_sets.size(); ++i)
	{
		VkDescriptorBufferInfo descriptor_buffer_info = {
			uniform_buffers[i],
			0,
			sizeof(UniformBufferObject)
		};

		VkDescriptorImageInfo descriptor_image_info = {
			texture_sampler,
			texture_image_view,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		};

		VkWriteDescriptorSet uniform_buffer_object_write_descriptor_set = {
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			nullptr,
			descriptor_sets[i],
			0,
			0,
			1,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			nullptr,
			&descriptor_buffer_info,
			nullptr
		};

		VkWriteDescriptorSet sampler_write_descriptor_set = {
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			nullptr,
			descriptor_sets[i],
			1,
			0,
			1,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			&descriptor_image_info,
			nullptr,
			nullptr
		};

		std::array<VkWriteDescriptorSet, 2> write_descriptor_sets = {
			uniform_buffer_object_write_descriptor_set,
			sampler_write_descriptor_set
		};

		vkUpdateDescriptorSets(device, (uint32_t)write_descriptor_sets.size(), write_descriptor_sets.data(), 0, nullptr);
	}
}

void ModelRenderSystem::recreateSwapchain()
{
	if (present_thread.joinable())
		present_thread.join();
	vkDeviceWaitIdle(device);

	releaseSwapchain();

	createSwapchain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createDepthBuffer();
	createFramebuffers();
	allocateCommandBuffers();
}

void ModelRenderSystem::release()
{
	vkDeviceWaitIdle(device);

	releaseSwapchain();

	vkDestroySampler(device, texture_sampler, nullptr);

	vkDestroyImageView(device, texture_image_view, nullptr);

	vkDestroyImage(device, texture_image, nullptr);
	vkFreeMemory(device, texture_image_memory, nullptr);

	delete vma;

	vkDestroyDescriptorSetLayout(device, descriptor_set_layout, nullptr);

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
	vkDestroyImageView(device, depth_image_view, nullptr);
	vkDestroyImage(device, depth_image, nullptr);
	vkFreeMemory(device, depth_image_memory, nullptr);

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

VkDevice ModelRenderSystem::getDevice() const
{
	return device;
}

VideoMemoryAllocator * ModelRenderSystem::getVideoMemoryAllocator() const
{
	return vma;
}

VkPhysicalDevice ModelRenderSystem::getPhysicalDevice() const
{
	return physical_device;
}

VkCommandPool ModelRenderSystem::getCommandPool() const
{
	return command_pool;
}

VkRenderPass ModelRenderSystem::getRenderPass() const
{
	return render_pass;
}

VkPipelineLayout ModelRenderSystem::getPipelineLayout() const
{
	return pipeline_layout;
}

VkPipeline ModelRenderSystem::getGraphicsPipeline() const
{
	return graphics_pipeline;
}

VkDescriptorPool ModelRenderSystem::getDescriptorPool() const
{
	return descriptor_pool;
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
	if (staging_regions.size())
	{
		copyBuffers(staging_buffer, vma->buffer, staging_regions);
		staging_regions.clear();
		staging_offset = 0;
	}

	vkWaitForFences(device, 1, &in_flight_fences[current_frame_index], VK_TRUE, UINT64_MAX);

	time += dt;

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

	if (images_in_flight[image_index] != VK_NULL_HANDLE && images_in_flight[image_index] != in_flight_fences[current_frame_index])
		vkWaitForFences(device, 1, &images_in_flight[image_index], VK_TRUE, UINT64_MAX);
	images_in_flight[image_index] = in_flight_fences[current_frame_index];

	recordCommandBuffer(image_index);

	updateUniformBuffer(image_index);
	for (auto& model : level->models.components)
		model.updateUniformBuffer(this, image_index);

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

	if (present_thread.joinable())
		present_thread.join();

	if (vkQueueSubmit(graphics_queue, 1, &submit_info, in_flight_fences[current_frame_index]))
		throw std::runtime_error("failed to submit draw command buffer.");

	present_thread = std::thread([this, signal_semaphores, image_index]()
	{
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
	});

	++current_frame_index %= max_frames_in_flight;
}
