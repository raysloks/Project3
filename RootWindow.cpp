#include "RootWindow.h"

#include <ranges>

#include "CustomBehaviour.h"

RootWindow::RootWindow()
{
	root = this;
}

void RootWindow::setFocus(const std::shared_ptr<Window>& window)
{
	if (window != getFocus())
	{
		std::vector<std::weak_ptr<Window>> prev_focus = std::move(focus);
		focus.clear();
		for (auto i = window.get(); i != nullptr; i = i->parent)
			focus.push_back(i->weak_from_this());
		FocusLostEvent lost_event;
		for (size_t i = 0; i < prev_focus.size(); ++i)
		{
			auto locked = prev_focus[i].lock();
			if (locked)
			{
				std::shared_ptr<Window> current;
				if (i < focus.size())
					current = focus[i].lock();
				if (locked != current)
					locked->onEvent(lost_event);
			}
		}
		FocusGainedEvent gained_event;
		for (size_t i = 0; i < focus.size(); ++i)
		{
			auto locked = focus[i].lock();
			if (locked)
			{
				std::shared_ptr<Window> prev;
				if (i < prev_focus.size())
					prev = prev_focus[i].lock();
				if (locked != prev)
					locked->onEvent(gained_event);
			}
		}
	}
}

std::shared_ptr<Window> RootWindow::getFocus()
{
	for (auto& window : focus)
	{
		auto locked = window.lock();
		if (locked)
			return locked;
	}
	return nullptr;
}

void RootWindow::focusPrevious(bool wrap)
{
	auto focus = getFocus();
	if (focus)
	{
		auto parent = focus->parent;
		auto it = std::find(parent->children.rbegin(), parent->children.rend(), focus);
		while (it != parent->children.rend())
		{
			++it;
			if (it == parent->children.rend())
				if (wrap)
					it = parent->children.rbegin();
				else
					break;
			if ((*it)->clickable || *it == focus)
				break;
		}
		if (it != parent->children.rend())
			setFocus(*it);
		else
			setFocus(parent->shared_from_this());
	}
}

void RootWindow::focusNext(bool wrap)
{
	auto focus = getFocus();
	if (focus)
	{
		auto parent = focus->parent;
		auto it = std::find(parent->children.begin(), parent->children.end(), focus);
		while (it != parent->children.end())
		{
			++it;
			if (it == parent->children.end())
				if (wrap)
					it = parent->children.begin();
				else
					break;
			if ((*it)->clickable || *it == focus)
				break;
		}
		if (it != parent->children.end())
			setFocus(*it);
		else
			setFocus(parent->shared_from_this());
	}
}

void RootWindow::processCursorMoveEvent(const CursorMoveEvent& event)
{
	auto focus = getFocus();
	if (focus)
		focus->processEventSelfChildrenParent(event);

	auto hover = getAtPosition(event.cursor_position);
	auto locked_mouseover = mouseover.lock();
	if (hover != locked_mouseover)
	{
		CursorLeaveEvent leave_event;
		leave_event.cursor_position = event.cursor_position;
		leave_event.cursor_index = event.cursor_index;
		if (locked_mouseover)
			locked_mouseover->onEvent(leave_event);

		CursorEnterEvent enter_event;
		enter_event.cursor_position = event.cursor_position;
		enter_event.cursor_index = event.cursor_index;
		if (hover)
			hover->onEvent(enter_event);

		mouseover = hover;
		if (hover)
			CustomBehaviour::engine->setCursor((SDL_SystemCursor)hover->cursor);
	}
}

bool RootWindow::processKeyDownEvent(const KeyDownEvent& event)
{
	auto focus = getFocus();
	if (focus)
		if (focus->processEventSelfChildrenParent(event))
			return true;
	return false;
}

bool RootWindow::processKeyUpEvent(const KeyUpEvent& event)
{
	auto focus = getFocus();
	if (focus)
		if (focus->processEventSelfChildrenParent(event))
			return true;
	return false;
}

bool RootWindow::processButtonDownEvent(const KeyDownEvent& event)
{
	auto hover = getAtPosition(event.cursor_position);
	setFocus(hover);
	if (hover)
		if (hover->processEventSelfChildrenParent(event))
			return true;
	return false;
}

bool RootWindow::processButtonUpEvent(const KeyUpEvent& event)
{
	auto focus = getFocus();
	if (focus)
		if (focus->processEventSelfChildrenParent(event))
			return true;
	return false;
}

bool RootWindow::processTextInputEvent(const TextInputEvent& event)
{
	auto focus = getFocus();
	if (focus)
		if (focus->processEventSelfChildrenParent(event))
			return true;
	return false;
}
