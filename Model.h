#pragma once

#include "Resource.h"

#include <array>

#include "Vec2.h"
#include "Vec3.h"

struct VkVertexInputBindingDescription;
struct VkVertexInputAttributeDescription;

class ModelRenderSystem;
class VideoMemoryAllocator;

class Model :
	public Resource<Model>
{
public:
	Model();
	~Model();

	static std::shared_ptr<Model> load(const std::string & fname);

	static VkVertexInputBindingDescription getBindingDescription();
	static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

	void getBufferOffsets(ModelRenderSystem * mrs, size_t& vertex_buffer_offset, size_t& index_buffer_offset);

	class Vertex
	{
	public:
		Vec3 position;
		Vec2 uv;
		Vec3 normal;
		uint8_t bones[4];
		float weights[4];
	};

	class Triangle
	{
	public:
		uint16_t indices[3];
	};

	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;

	VideoMemoryAllocator * vma;
	size_t vertex_buffer_offset;
	size_t index_buffer_offset;
};
