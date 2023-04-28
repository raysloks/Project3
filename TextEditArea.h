#pragma once

#include "Window.h"

#include "TextArea.h"

class TextEditArea :
	public Window
{
public:
	TextEditArea();

	bool onEvent(const KeyDownEvent& event);
	bool onEvent(const KeyUpEvent& event);
	bool onEvent(const TextInputEvent& event);
	bool onEvent(const CursorMoveEvent& event);
	bool onEvent(const FocusGainedEvent& event);
	bool onEvent(const FocusLostEvent& event);

protected:
	std::weak_ptr<TextArea> text_area;
	std::weak_ptr<Window> cursor_window;
	int64_t start, end;
	bool focus, selecting;

	bool multiline;

	void update();
	void updateCursor();
};

