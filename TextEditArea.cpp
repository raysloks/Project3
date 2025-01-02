#include "TextEditArea.h"

#include "Font.h"
#include "ModelRenderer.h"

#include "RootWindow.h"

#include "Utf8.h"

TextEditArea::TextEditArea()
{
	clickable = true;
	start = 0;
	end = 0;
	focus = false;
	selecting = false;
	cursor = SDL_SYSTEM_CURSOR_IBEAM;

	multiline = false;

	auto text_area_shared = std::make_shared<TextArea>();
	text_area_shared->font = Font::load("Lora-Regular.ttf");
	addChild(text_area_shared);
	text_area = text_area_shared;

	auto cursor_window_shared = std::make_shared<Window>(0.0f, 0.0f, Vec2(0.0f, 0.0f), Vec2(1.0f, 16.0f));
	cursor_window_shared->model = std::make_shared<ModelRenderer>("offset_plane.mdl", "pixel.png", "", 1);
	cursor_window_shared->hidden = true;
	text_area_shared->text_window.lock()->addChild(cursor_window_shared);
	cursor_window = cursor_window_shared;
}

bool TextEditArea::onEvent(const KeyDownEvent & event)
{
	auto text_area_shared = text_area.lock();
	if (text_area_shared == nullptr)
		return false;

	if (event.key == std::numeric_limits<uint64_t>::max() - 1)
	{
		start = text_area_shared->getIndexFromPosition(event.cursor_position);
		end = start;
		selecting = true;
		updateCursor();
		return true;
	}

	switch (event.key)
	{
	case SDLK_BACKSPACE:
	{
		if (start == end)
			start = getPreviousCodePointPosition(text_area_shared->text, start);
		else if (start > end)
			std::swap(start, end);
		text_area_shared->text.erase(start, end - start);
		end = start;
		update();
		return true;
	}
	break;
	case SDLK_DELETE:
	{
		if (start == end)
			end = getNextCodePointPosition(text_area_shared->text, end);
		else if (start > end)
			std::swap(start, end);
		text_area_shared->text.erase(start, end - start);
		end = start;
		update();
		return true;
	}
	break;
	case SDLK_RETURN:
	{
		if (multiline || (event.mod & KMOD_SHIFT))
		{
			TextInputEvent text_input_event;
			text_input_event.cursor_position = event.cursor_position;
			text_input_event.text = "\n";
			return onEvent(text_input_event);
		}
		else
		{
			root->focusNext(false);
			return true;
		}
	}
	break;
	case SDLK_LEFT:
	{
		start = getPreviousCodePointPosition(text_area_shared->text, start);
		if (!(event.mod & KMOD_SHIFT))
			end = start;
		updateCursor();
		return true;
	}
	break;
	case SDLK_RIGHT:
	{
		start = getNextCodePointPosition(text_area_shared->text, start);
		if (!(event.mod & KMOD_SHIFT))
			end = start;
		updateCursor();
		return true;
	}
	break;
	case SDLK_UP:
	{
		auto text_window_shared = text_area_shared->text_window.lock();
		if (text_window_shared && text_window_shared->model && text_window_shared->model->textures.size())
			start = text_window_shared->model->textures[0]->getIndexFromPosition(text_area_shared->text, text_area_shared->settings, text_window_shared->model->textures[0]->getAdvance(text_area_shared->text, text_area_shared->settings, start) + Vec2(0.0f, text_area_shared->settings.line_height * -1.5f));
		if (!(event.mod & KMOD_SHIFT))
			end = start;
		updateCursor();
		return true;
	}
	break;
	case SDLK_DOWN:
	{
		auto text_window_shared = text_area_shared->text_window.lock();
		if (text_window_shared && text_window_shared->model && text_window_shared->model->textures.size())
			start = text_window_shared->model->textures[0]->getIndexFromPosition(text_area_shared->text, text_area_shared->settings, text_window_shared->model->textures[0]->getAdvance(text_area_shared->text, text_area_shared->settings, start) + Vec2(0.0f, text_area_shared->settings.line_height * 0.5f));
		if (!(event.mod & KMOD_SHIFT))
			end = start;
		updateCursor();
		return true;
	}
	break;
	default:
		break;
	}
	return false;
}

bool TextEditArea::onEvent(const KeyUpEvent & event)
{
	auto text_area_shared = text_area.lock();
	if (text_area_shared == nullptr)
		return false;

	if (event.key == std::numeric_limits<uint64_t>::max() - 1 && selecting)
	{
		start = text_area_shared->getIndexFromPosition(event.cursor_position);
		selecting = false;
		return true;
	}
	return false;
}

bool TextEditArea::onEvent(const TextInputEvent & event)
{
	auto text_area_shared = text_area.lock();
	if (text_area_shared)
	{
		if (start != end)
		{
			if (start > end)
				std::swap(start, end);
			text_area_shared->text.erase(start, end - start);
		}
		text_area_shared->text.insert(start, event.text);
		start += event.text.size();
		end = start;
		update();
		return true;
	}
	return false;
}

bool TextEditArea::onEvent(const CursorMoveEvent & event)
{
	auto text_area_shared = text_area.lock();
	if (text_area_shared == nullptr)
		return false;

	if (selecting)
	{
		start = text_area_shared->getIndexFromPosition(event.cursor_position);
		updateCursor();
	}
	return false;
}

bool TextEditArea::onEvent(const FocusGainedEvent & event)
{
	SDL_StartTextInput();
	auto cursor_window_shared = cursor_window.lock();
	if (cursor_window_shared)
		cursor_window_shared->hidden = false;
	return false;
}

bool TextEditArea::onEvent(const FocusLostEvent & event)
{
	SDL_StopTextInput();
	auto cursor_window_shared = cursor_window.lock();
	if (cursor_window_shared)
		cursor_window_shared->hidden = true;
	return false;
}

void TextEditArea::update()
{
	auto text_area_shared = text_area.lock();
	if (text_area_shared)
	{
		text_area_shared->update();
		updateCursor();
	}
}

void TextEditArea::updateCursor()
{
	auto text_area_shared = text_area.lock();
	auto cursor_window_shared = cursor_window.lock();
	if (text_area_shared && cursor_window_shared)
	{
		auto text_window_shared = text_area_shared->text_window.lock();
		if (text_window_shared && text_window_shared->model && text_window_shared->model->textures.size())
			cursor_window_shared->setSizeAnchorOffset(Vec2(1.0f, text_area_shared->size.y), text_window_shared->model->textures[0]->getAdvance(text_area_shared->text, text_area_shared->settings, start) + Vec2(0.5f, -text_area_shared->size.y * 0.5f), Vec2(0.0f, 1.0f));
	}
}
