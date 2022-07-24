#pragma once

#include <vector>
#include <memory>

#include <vulkan/vulkan.h>

#include "Vec2.h"
#include "WindowEvents.h"

class ModelRenderSystem;
class ModelRenderer;
class RenderContext;

class Window
{
public:
	Window();
	Window(const Vec2& minAnchor, const Vec2& maxAnchor, const Vec2& minOffset, const Vec2& maxOffset);
	virtual ~Window();

	void addChild(const std::shared_ptr<Window>& child);
	void removeChild(const std::shared_ptr<Window>& child);

	Window * getParent() const;

	Vec2 getMin() const;
	Vec2 getMax() const;

	void setAnchorOffset(const Vec2& minAnchor, const Vec2& maxAnchor, const Vec2& minOffset, const Vec2& maxOffset);

	void getRenderingModels(RenderContext& render_context);
	void updateUniformBuffers(const RenderContext& render_context);

	Vec2 minAnchor, maxAnchor;
	Vec2 minOffset, maxOffset;

	std::shared_ptr<ModelRenderer> model;

	template <class T>
	bool processEvent(const T& event)
	{
		for (auto it = children.rbegin(); it != children.rend(); ++it)
			if ((*it)->processEvent(event))
				return true;
		return onEvent(event);
	}

	virtual bool onEvent(const KeyDownEvent& event);
	virtual bool onEvent(const KeyUpEvent& event);
	virtual bool onEvent(const TextInputEvent& event);

//private:
	std::vector<std::shared_ptr<Window>> children;
	Window * parent;
};
