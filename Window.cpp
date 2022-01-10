#include "Window.h"

#include "ModelRenderer.h"
#include "RenderContext.h"

bool Window::onKeyDown(uint64_t key)
{
	return false;
}

bool Window::onKeyUp(uint64_t key)
{
	return false;
}

Vec2 Window::getMin() const
{
	auto locked = parent.lock();
	if (locked)
	{
		auto min = locked->getMin();
		auto max = locked->getMax();
		return max * minAnchor + min * (Vec2(1.0f) - minAnchor) + minOffset;
	}
	else
	{
		return minOffset;
	}
}

Vec2 Window::getMax() const
{
	auto locked = parent.lock();
	if (locked)
	{
		auto min = locked->getMin();
		auto max = locked->getMax();
		return max * maxAnchor + min * (Vec2(1.0f) - maxAnchor) + maxOffset;
	}
	else
	{
		return maxOffset;
	}
}

void Window::getRenderingModels(RenderContext& render_context)
{
	for (auto& child : children)
		child->getRenderingModels(render_context);
	if (model)
	{
		auto rendering_model = model->getRenderingModel(render_context);
		if (rendering_model)
			render_context.addRenderingModel(rendering_model);
	}
}

void Window::updateUniformBuffers(const RenderContext& render_context)
{
	for (auto& child : children)
		child->updateUniformBuffers(render_context);
	if (model)
		model->updateUniformBuffer(render_context);
}
