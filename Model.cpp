#include "Model.h"

#include <thread>
#include <fstream>
#include <sstream>

#include <vulkan/vulkan.h>

#include "ModelRenderSystem.h"
#include "VideoMemoryAllocator.h"

Model::Model()
{
	loaded = false;
	vma = nullptr;
}

Model::~Model()
{
	if (vma)
	{
		size_t vertex_buffer_size = sizeof(vertices[0]) * vertices.size();
		size_t index_buffer_size = sizeof(triangles[0]) * triangles.size();

		vma->deallocate(vertex_buffer_size, vertex_buffer_offset);
		vma->deallocate(index_buffer_size, index_buffer_offset);
	}
}

std::shared_ptr<Model> Model::load(const std::string & fname)
{
	auto model = std::make_shared<Model>();
	std::thread t([model, fname]()
		{
			std::ifstream in(BaseResource::data_location + fname, std::ios::in | std::ios::binary);

			if (in)
			{
				std::string buffer;
				in.seekg(0, std::ios::end);
				buffer.resize(in.tellg());
				in.seekg(0, std::ios::beg);
				in.read(&buffer[0], buffer.size());
				in.close();

				std::stringstream ss(buffer);

				uint8_t flags;
				ss.read((char*)&flags, sizeof(flags));

				uint16_t vertex_count;
				ss.read((char*)&vertex_count, sizeof(vertex_count));
				model->vertices.resize(vertex_count);
				ss.read((char*)model->vertices.data(), sizeof(Vertex) * vertex_count);

				uint16_t triangle_count;
				ss.read((char*)&triangle_count, sizeof(triangle_count));
				model->triangles.resize(triangle_count);
				ss.read((char*)model->triangles.data(), sizeof(Triangle) * triangle_count);
			}

			model->loaded = true;
			model->loaded.notify_all();
		});
	t.detach();

	return model;
}

VkVertexInputBindingDescription Model::getBindingDescription()
{
	VkVertexInputBindingDescription vertex_input_binding_description = {
		0,
		sizeof(Vertex),
		VK_VERTEX_INPUT_RATE_VERTEX
	};
	return vertex_input_binding_description;
}

std::vector<VkVertexInputAttributeDescription> Model::getAttributeDescriptions()
{
	std::vector<VkVertexInputAttributeDescription> vertex_input_attribute_descriptions;
	vertex_input_attribute_descriptions.push_back({
		0,
		0,
		VK_FORMAT_R32G32B32_SFLOAT,
		offsetof(Vertex, position)
		});
	vertex_input_attribute_descriptions.push_back({
		1,
		0,
		VK_FORMAT_R32G32_SFLOAT,
		offsetof(Vertex, uv)
		});
	vertex_input_attribute_descriptions.push_back({
		2,
		0,
		VK_FORMAT_R32G32B32_SFLOAT,
		offsetof(Vertex, normal)
		});
	vertex_input_attribute_descriptions.push_back({
		3,
		0,
		VK_FORMAT_R8G8B8A8_UINT,
		offsetof(Vertex, bones)
		});
	vertex_input_attribute_descriptions.push_back({
		4,
		0,
		VK_FORMAT_R32G32B32A32_SFLOAT,
		offsetof(Vertex, weights)
		});
	return vertex_input_attribute_descriptions;
}

void Model::getBufferOffsets(ModelRenderSystem * mrs, size_t& vertex_buffer_offset, size_t& index_buffer_offset)
{
	if (vma == nullptr)
	{
		vma = mrs->getVideoMemoryAllocator();

		size_t vertex_buffer_size = sizeof(vertices[0]) * vertices.size();
		size_t index_buffer_size = sizeof(triangles[0]) * triangles.size();

		this->vertex_buffer_offset = vma->allocate(vertex_buffer_size);
		this->index_buffer_offset = vma->allocate(index_buffer_size);

		mrs->stageBufferData(vertices.data(), vertex_buffer_size, this->vertex_buffer_offset);
		mrs->stageBufferData(triangles.data(), index_buffer_size, this->index_buffer_offset);
	}

	vertex_buffer_offset = this->vertex_buffer_offset;
	index_buffer_offset = this->index_buffer_offset;
}
