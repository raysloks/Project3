#pragma once

#include <vector>
#include <memory>

#include <vulkan/vulkan.h>

#include "Vec2.h"
#include "WindowEvents.h"

class ModelRenderSystem;
class ModelRenderer;
class RenderContext;
class RootWindow;

class Window :
	public std::enable_shared_from_this<Window>
{
public:
	Window();
	Window(const Vec2& minAnchor, const Vec2& maxAnchor, const Vec2& minOffset, const Vec2& maxOffset);
	virtual ~Window();

	void addChild(const std::shared_ptr<Window>& child, intmax_t index = -1);
	void removeChild(const std::shared_ptr<Window>& child);
	void removeSelf();

	Window * getParent() const;
	RootWindow * getRoot() const;

	Vec2 getMin() const;
	Vec2 getMax() const;
	Vec2 getCenter() const;

	void setAnchorOffset(const Vec2& minAnchor, const Vec2& maxAnchor, const Vec2& minOffset, const Vec2& maxOffset);
	void setSizeAnchorOffset(const Vec2& size, const Vec2& offset = Vec2(0.0f, 0.0f), const Vec2& anchor = Vec2(0.5f, 0.5f));

	void getRenderingModels(RenderContext& render_context);
	void updateUniformBuffers(const RenderContext& render_context);

	bool containsPosition(const Vec2& position) const;
	std::shared_ptr<Window> getAtPosition(const Vec2& position);

	Vec2 minAnchor, maxAnchor;
	Vec2 minOffset, maxOffset;

	std::shared_ptr<ModelRenderer> model;

	bool hidden;

	bool clickable;
	bool clipping;

	uint64_t cursor;

	template <class T>
	std::shared_ptr<Window> processEventSelfParent(const T& event)
	{
		if (onEvent(event))
			return shared_from_this();
		if (parent)
			return parent->processEventSelfParent(event);
		return nullptr;
	}

	template <class T>
	std::shared_ptr<Window> processEventSelfChildrenParent(const T& event, Window * ignore = nullptr)
	{
		if (onEvent(event))
			return shared_from_this();
		std::vector<std::shared_ptr<Window>> copy = children;
		for (auto it = copy.rbegin(); it != copy.rend(); ++it)
			if (it->get() != ignore)
			{
				auto value = (*it)->processEventSelfChildren(event);
				if (value)
					return value;
			}
		if (parent)
			return parent->processEventSelfChildrenParent(event, this);
		return nullptr;
	}

	template <class T>
	std::shared_ptr<Window> processEventSelfChildren(const T& event)
	{
		if (onEvent(event))
			return shared_from_this();
		std::vector<std::shared_ptr<Window>> copy = children;
		for (auto it = copy.rbegin(); it != copy.rend(); ++it)
		{
			auto value = (*it)->processEventSelfChildren(event);
			if (value)
				return value;
		}
		return nullptr;
	}

	virtual bool onEvent(const KeyDownEvent& event);
	virtual bool onEvent(const KeyUpEvent& event);
	virtual bool onEvent(const TextInputEvent& event);
	virtual bool onEvent(const CursorMoveEvent& event);
	virtual bool onEvent(const CursorEnterEvent& event);
	virtual bool onEvent(const CursorLeaveEvent& event);
	virtual bool onEvent(const FocusGainedEvent& event);
	virtual bool onEvent(const FocusLostEvent& event);
	virtual bool onEvent(const LayoutEvent& event);

//private:

	void setRoot(RootWindow * root);

	std::vector<std::shared_ptr<Window>> children;
	Window * parent;
	RootWindow * root;
};
