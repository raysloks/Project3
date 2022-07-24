#include "Window.h"

#include "ModelRenderer.h"
#include "RenderContext.h"

Window::Window()
{
	parent = nullptr;
	maxAnchor = 1.0f;
}

Window::Window(const Vec2& minAnchor, const Vec2& maxAnchor, const Vec2& minOffset, const Vec2& maxOffset) : minAnchor(minAnchor), maxAnchor(maxAnchor), minOffset(minOffset), maxOffset(maxOffset)
{
	parent = nullptr;
}

Window::~Window()
{
	for (auto& child : children)
		child->parent = nullptr;
}

void Window::addChild(const std::shared_ptr<Window>& child)
{
	if (child->parent)
	{
		auto it = std::find(child->parent->children.begin(), child->parent->children.end(), child);
		if (it != child->parent->children.end())
			child->parent->children.erase(it);
	}
	children.push_back(child);
	child->parent = this;
}

void Window::removeChild(const std::shared_ptr<Window>& child)
{
	auto it = std::find(children.begin(), children.end(), child);
	if (it != children.end())
	{
		children.erase(it);
		child->parent = nullptr;
	}
}

Window * Window::getParent() const
{
	return parent;
}

Vec2 Window::getMin() const
{
	if (parent)
	{
		auto min = parent->getMin();
		auto max = parent->getMax();
		return max * minAnchor + min * (Vec2(1.0f) - minAnchor) + minOffset;
	}
	else
	{
		return minOffset;
	}
}

Vec2 Window::getMax() const
{
	if (parent)
	{
		auto min = parent->getMin();
		auto max = parent->getMax();
		return max * maxAnchor + min * (Vec2(1.0f) - maxAnchor) + maxOffset;
	}
	else
	{
		return maxOffset;
	}
}

void Window::setAnchorOffset(const Vec2& minAnchor, const Vec2& maxAnchor, const Vec2& minOffset, const Vec2& maxOffset)
{
	this->minAnchor = minAnchor;
	this->maxAnchor = maxAnchor;
	this->minOffset = minOffset;
	this->maxOffset = maxOffset;
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
	{
		model->setDirty(); // nasty tmp

		auto min = getMin();
		auto max = getMax();
		auto size = max - min;
		model->uniform_buffer_object.model = Matrix4::Scale(Vec3(size.x, -size.y, 1.0f)) * Matrix4::Translation(Vec3(min.x, -min.y));
		model->updateUniformBuffer(render_context);
	}
}

bool Window::onEvent(const KeyDownEvent & event)
{
	return false;
}

bool Window::onEvent(const KeyUpEvent & event)
{
	return false;
}

bool Window::onEvent(const TextInputEvent & event)
{
	return false;
}
