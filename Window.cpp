#include "Window.h"

#include "ModelRenderer.h"
#include "RenderContext.h"

Window::Window()
{
	parent = nullptr;
	root = nullptr;
	maxAnchor = 1.0f;
	hidden = false;
	clickable = false;
	clipping = false;
	cursor = SDL_SYSTEM_CURSOR_ARROW;
}

Window::Window(const Vec2& minAnchor, const Vec2& maxAnchor, const Vec2& minOffset, const Vec2& maxOffset) : minAnchor(minAnchor), maxAnchor(maxAnchor), minOffset(minOffset), maxOffset(maxOffset)
{
	parent = nullptr;
	root = nullptr;
	hidden = false;
	clickable = false;
	clipping = false;
	cursor = SDL_SYSTEM_CURSOR_ARROW;
}

Window::~Window()
{
	for (auto& child : children)
	{
		child->parent = nullptr;
		child->setRoot(nullptr);
	}
}

void Window::addChild(const std::shared_ptr<Window>& child, intmax_t index)
{
	if (child->parent)
	{
		auto it = std::find(child->parent->children.begin(), child->parent->children.end(), child);
		if (it != child->parent->children.end())
			child->parent->children.erase(it);
	}
	if (index < 0)
		children.insert(children.end() + index + 1, child);
	else
		children.insert(children.begin() + index, child);
	child->parent = this;
	child->setRoot(root);
}

void Window::removeChild(const std::shared_ptr<Window>& child)
{
	auto it = std::find(children.begin(), children.end(), child);
	if (it != children.end())
	{
		children.erase(it);
		child->parent = nullptr;
		child->setRoot(nullptr);
	}
}

void Window::removeSelf()
{
	if (parent)
		parent->removeChild(shared_from_this());
}

Window * Window::getParent() const
{
	return parent;
}

RootWindow * Window::getRoot() const
{
	return root;
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

Vec2 Window::getCenter() const
{
	return (getMin() + getMax()) * 0.5f;
}

void Window::setAnchorOffset(const Vec2& minAnchor, const Vec2& maxAnchor, const Vec2& minOffset, const Vec2& maxOffset)
{
	this->minAnchor = minAnchor;
	this->maxAnchor = maxAnchor;
	this->minOffset = minOffset;
	this->maxOffset = maxOffset;
}

void Window::setSizeAnchorOffset(const Vec2& size, const Vec2& offset, const Vec2& anchor)
{
	minAnchor = anchor;
	maxAnchor = anchor;
	Vec2 half_size = size * 0.5f;
	minOffset = offset - half_size;
	maxOffset = offset + half_size;
}

void Window::getRenderingModels(RenderContext& render_context)
{
	if (hidden)
		return;

	if (clipping)
	{
		render_context.pushDynamicState();
		auto min = getMin();
		auto max = getMax();
		auto size = max - min;
		render_context.dynamic_state.scissor = { (int32_t)min.x, (int32_t)min.y, (uint32_t)size.x, (uint32_t)size.y };
	}

	for (auto& child : children)
		child->getRenderingModels(render_context);

	if (clipping)
		render_context.popDynamicState();

	if (model)
	{
		auto rendering_model = model->getRenderingModel(render_context);
		if (rendering_model)
			render_context.addRenderingModel(rendering_model);
	}
}

void Window::updateUniformBuffers(const RenderContext& render_context)
{
	if (hidden)
		return;

	for (auto& child : children)
		child->updateUniformBuffers(render_context);
	if (model)
	{
		model->setDirty(); // nasty tmp

		auto min = getMin();
		auto max = getMax();
		auto size = max - min;
		model->uniform_buffer_object.model = Matrix4::Scale(Vec3(size.x, size.y, 1.0f)) * Matrix4::Translation(Vec3(min.x, -max.y));
		model->updateUniformBuffer(render_context);
	}
}

bool Window::containsPosition(const Vec2& position) const
{
	return position >= getMin() && position <= getMax();
}

std::shared_ptr<Window> Window::getAtPosition(const Vec2& position)
{
	if (hidden)
		return nullptr;

	bool contains = containsPosition(position);
	if (contains || !clipping)
	{
		for (auto i = children.rbegin(); i != children.rend(); ++i)
		{
			auto window = (*i)->getAtPosition(position);
			if (window)
				return window;
		}
	}
	if (contains && clickable)
		return shared_from_this();
	else
		return nullptr;
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

bool Window::onEvent(const CursorMoveEvent & event)
{
	return false;
}

bool Window::onEvent(const CursorEnterEvent & event)
{
	return false;
}

bool Window::onEvent(const CursorLeaveEvent & event)
{
	return false;
}

bool Window::onEvent(const FocusGainedEvent & event)
{
	return false;
}

bool Window::onEvent(const FocusLostEvent & event)
{
	return false;
}

bool Window::onEvent(const LayoutEvent & event)
{
	return false;
}

void Window::setRoot(RootWindow * root)
{
	if (this->root != root)
	{
		this->root = root;
		for (auto& child : children)
			child->setRoot(root);
	}
}
