#pragma once

#include <vector>
#include <memory>

#include <vulkan/vulkan.h>

#include "Vec2.h"

class ModelRenderSystem;
class ModelRenderer;
class RenderContext;

class Window
{
public:
	virtual bool onKeyDown(uint64_t key);
	virtual bool onKeyUp(uint64_t key);

	Vec2 getMin() const;
	Vec2 getMax() const;

	void getRenderingModels(RenderContext& render_context);
	void updateUniformBuffers(const RenderContext& render_context);

	Vec2 minAnchor, maxAnchor;
	Vec2 minOffset, maxOffset;

	std::vector<std::shared_ptr<Window>> children;
	std::weak_ptr<Window> parent;

	std::shared_ptr<ModelRenderer> model;
};
