#pragma once

#include "Window.h"

class Vec4;

class Border :
	public Window
{
public:
	Border(float inner_width, float outer_width = 0.0f);

	void setWidth(float inner_width, float outer_width = 0.0f);
	void setColor(const Vec4& color);
};

