#pragma once

#include <vector>
#include <memory>

#include "Vec2.h"

class SpriteRenderSystem;

class Window
{
public:
	virtual void onRender(SpriteRenderSystem * srs);
	virtual bool onKeyDown(uint64_t key);
	virtual bool onKeyUp(uint64_t key);

	Vec2 getMin() const;
	Vec2 getMax() const;

	Vec2 minAnchor, maxAnchor;
	Vec2 minOffset, maxOffset;

	std::vector<std::shared_ptr<Window>> children;
	std::weak_ptr<Window> parent;
};

