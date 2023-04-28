#pragma once

#include "Window.h"

class RootWindow :
	public Window
{
public:
	RootWindow();

	void setFocus(const std::shared_ptr<Window>& window);
	std::shared_ptr<Window> getFocus();

	void focusPrevious(bool wrap);
	void focusNext(bool wrap);

	void processCursorMoveEvent(const CursorMoveEvent& event);
	bool processKeyDownEvent(const KeyDownEvent& event);
	bool processKeyUpEvent(const KeyUpEvent& event);
	bool processButtonDownEvent(const KeyDownEvent& event);
	bool processButtonUpEvent(const KeyUpEvent& event);
	bool processTextInputEvent(const TextInputEvent& event);

private:
	std::vector<std::weak_ptr<Window>> focus;
	std::weak_ptr<Window> mouseover;
};

