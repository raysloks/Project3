#pragma once

#include "Window.h"

#include "TextSettings.h"

class Font;

class TextArea :
	public Window
{
public:
	TextArea();

	void update();

	int64_t getIndexFromPosition(const Vec2& position) const;

	std::weak_ptr<Window> text_window;
	std::shared_ptr<Font> font;
	std::string text;
	Vec2 align, size;
	TextSettings settings;
};

