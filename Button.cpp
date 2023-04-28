#include "Button.h"

#include "NineSlice.h"
#include "SpriteSheet.h"
#include "Font.h"

Button::Button(const std::string& label)
{
	clickable = true;

	hover = false;
	press = false;

	auto bg_default_shared = std::make_shared<NineSlice>(SpriteSheet::load("button.png"));
	addChild(bg_default_shared);
	bg_default = bg_default_shared;

	auto bg_hover_shared = std::make_shared<NineSlice>(SpriteSheet::load("button_hover.png"));
	addChild(bg_hover_shared);
	bg_hover = bg_hover_shared;

	auto bg_press_shared = std::make_shared<NineSlice>(SpriteSheet::load("button_press.png"));
	addChild(bg_press_shared);
	bg_press = bg_press_shared;

	auto text_area_shared = std::make_shared<TextArea>();
	text_area_shared->text = label;
	text_area_shared->font = Font::load("Lora-Regular.ttf");
	text_area_shared->align = Vec2(0.5f);
	text_area_shared->settings.justification = Vec2(0.5f, 0.0f);
	text_area_shared->settings.color = Vec4(0.0f, 1.0f);
	addChild(text_area_shared);
	text_area_shared->update();
	text_area = text_area_shared;

	update();
}

bool Button::onEvent(const KeyDownEvent& event)
{
	if (event.key == std::numeric_limits<uint64_t>::max() - 1)
	{
		press = true;
		update();
		return true;
	}
	return false;
}

bool Button::onEvent(const KeyUpEvent& event)
{
	if (event.key == std::numeric_limits<uint64_t>::max() - 1)
	{
		press = false;
		update();
		if (containsPosition(event.cursor_position))
			onClick();
		return true;
	}
	return false;
}

bool Button::onEvent(const CursorEnterEvent& event)
{
	hover = true;
	update();
	return false;
}

bool Button::onEvent(const CursorLeaveEvent& event)
{
	hover = false;
	update();
	return false;
}

void Button::onClick()
{
}

void Button::update()
{
	auto bg_default_shared = bg_default.lock();
	if (bg_default_shared)
		bg_default_shared->hidden = hover || press;
	auto bg_hover_shared = bg_hover.lock();
	if (bg_hover_shared)
		bg_hover_shared->hidden = !hover || press;
	auto bg_press_shared = bg_press.lock();
	if (bg_press_shared)
		bg_press_shared->hidden = !press;
}
