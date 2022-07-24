#pragma once

#include "Window.h"

class Font;

class TextArea :
	public Window
{
public:
	void update();

	std::shared_ptr<Font> font;
	std::string text;
};

