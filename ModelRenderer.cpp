#include "ModelRenderer.h"

#include <stdexcept>

#include "ModelRenderSystem.h"
#include "VideoMemoryAllocator.h"

#include "RenderingModel.h"
#include "RenderContext.h"

ModelRenderer::ModelRenderer()
{
	uniform_vma = nullptr;
	camera_index = 0;
	uniform_buffer_object.color = Vec4(1.0f);
}

ModelRenderer::ModelRenderer(const std::shared_ptr<Model>& model, const std::vector<std::shared_ptr<SpriteSheet>>& textures, const std::shared_ptr<Animation>& animation, size_t camera_index) : model(model), textures(textures), animation(animation), camera_index(camera_index)
{
	uniform_vma = nullptr;
	uniform_buffer_object.color = Vec4(1.0f);
}

ModelRenderer::ModelRenderer(const std::shared_ptr<Model>& model, const std::shared_ptr<SpriteSheet>& texture, const std::shared_ptr<Animation>& animation, size_t camera_index) : model(model), textures({ texture }), animation(animation), camera_index(camera_index)
{
	uniform_vma = nullptr;
	uniform_buffer_object.color = Vec4(1.0f);
}

ModelRenderer::ModelRenderer(const std::string& model, const std::string& texture, const std::string& animation, size_t camera_index)
{
	this->model = Model::get(model);
	this->textures = { SpriteSheet::get(texture) };
	if (animation.size())
		this->animation = Animation::get(animation);
	uniform_vma = nullptr;
	this->camera_index = camera_index;
	uniform_buffer_object.color = Vec4(1.0f);
}

ModelRenderer::~ModelRenderer()
{
}

std::shared_ptr<RenderingModel> ModelRenderer::getRenderingModel(const RenderContext& render_context)
{
	if (!rendering_model || rendering_model->getGraphicsPipeline() != render_context.getGraphicsPipeline() || rendering_model->getDynamicState() != render_context.dynamic_state)
	{
		if (!model->loaded)
			return rendering_model;
		for (auto& texture : textures)
			if (!texture->loaded)
				return rendering_model;
		if (animation && !animation->loaded)
			return rendering_model;
		if (model->vertices.size() && model->triangles.size())
		{
			rendering_model.reset(new RenderingModel(model, textures, getUniformBufferObjectSize(), render_context.getModelRenderSystem(), render_context.getGraphicsPipeline(), camera_index, render_context.dynamic_state));
			dirty.clear();
			dirty.resize(render_context.getModelRenderSystem()->getDescriptorSetCount(), true);
		}
	}
	return rendering_model;
}

void ModelRenderer::updateUniformBuffer(const RenderContext& render_context)
{
	size_t current_image_index = render_context.getImageIndex();

	if (current_image_index >= dirty.size())
		return;

	if (dirty[current_image_index] && rendering_model)
	{
		dirty[current_image_index] = false;

		if (entity)
			uniform_buffer_object.model = transform * entity->getTransform();

		if (animation)
		{
			uniform_buffer_object.bones[0] = Matrix4();
			if (animation->loaded)
			{
				if (pose.bones.empty())
					pose = animation->base_pose;

				std::vector<Matrix4> transforms(256);
				transforms[255] = Matrix4();
				pose.apply(transforms.data());
				for (size_t i = 0; i < animation->base_pose_inverse_transforms.size(); ++i)
				{
					uniform_buffer_object.bones[i] = animation->base_pose_inverse_transforms[i] * transforms[i];
				}
			}
		}

		rendering_model->stageUniformBufferData(&uniform_buffer_object, current_image_index);
	}
}

void ModelRenderer::setDirty()
{
	for (size_t i = 0; i < dirty.size(); ++i)
		dirty[i] = true;
}

size_t ModelRenderer::getUniformBufferObjectSize() const
{
	if (animation)
	{
		if (animation->loaded)
		{
			return sizeof(uniform_buffer_object) - sizeof(uniform_buffer_object.bones) + sizeof(Matrix4) * animation->base_pose.bones.size();
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return sizeof(uniform_buffer_object) - sizeof(uniform_buffer_object.bones) + sizeof(Matrix4);
	}
}
