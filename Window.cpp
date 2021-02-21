#include "Window.h"

void Window::onRender(SpriteRenderSystem * srs)
{
}

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
