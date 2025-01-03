#pragma once

#include "Window.h"

#include "TextSettings.h"

class Font;
class SpriteSheet;

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
	Vec2 align;
	TextSettings settings;

	std::shared_ptr<SpriteSheet> atlas;
	Vec2 size;
	float outline;
};

