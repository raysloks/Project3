#pragma once

#include "Window.h"

#include "TextArea.h"

class Button :
	public Window
{
public:
	Button(const std::string& label = "");

	bool onEvent(const KeyDownEvent& event);
	bool onEvent(const KeyUpEvent& event);
	bool onEvent(const CursorEnterEvent& event);
	bool onEvent(const CursorLeaveEvent& event);

	virtual void onClick();

protected:
	std::weak_ptr<Window> bg_default, bg_hover, bg_press;
	std::weak_ptr<TextArea> text_area;

	bool hover, press;

	void update();
};

