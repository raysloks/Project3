#include "InputSystem.h"

#include <SDL.h>

#include "Engine.h"
#include "ModelRenderSystem.h"

void InputSystem::processKeyDownEvent(SDL_KeyboardEvent & event)
{
	processUnfilteredKeyDown(event.keysym.sym);
	processUnfilteredKeyDown(keyBindings.getAction(event.keysym.sym));

	KeyDownEvent ev;
	ev.cursor_position = cursorPosition;
	ev.key = event.keysym.sym;
	ev.mod = event.keysym.mod;
	ev.repeat = event.repeat;
	if (engine->mrs->ui->processKeyDownEvent(ev))
		return;

	// ignore auto-repeated key down events
	if (event.repeat)
		return;

	processKeyDown(ev.key);

	uint64_t action = keyBindings.getAction(ev.key);
	ev.key = action;
	if (engine->mrs->ui->processKeyDownEvent(ev))
		return;
	processKeyDown(action);
}

void InputSystem::processKeyUpEvent(SDL_KeyboardEvent & event)
{
	KeyUpEvent ev;
	ev.cursor_position = cursorPosition;
	ev.key = event.keysym.sym;
	ev.mod = event.keysym.mod;
	if (engine->mrs->ui->processKeyUpEvent(ev))
		return;
	processKeyUp(ev.key);

	uint64_t action = keyBindings.getAction(ev.key);
	ev.key = action;
	if (engine->mrs->ui->processKeyUpEvent(ev))
		return;
	processKeyUp(action);
}

void InputSystem::processMouseMoveEvent(SDL_MouseMotionEvent & event)
{
	cursorPosition.x = event.x;
	cursorPosition.y = event.y;
	CursorMoveEvent ev;
	ev.cursor_index = 0;
	ev.cursor_position = cursorPosition;
	engine->mrs->ui->processCursorMoveEvent(ev);
}

void InputSystem::processMouseWheelEvent(SDL_MouseWheelEvent & event)
{
}

void InputSystem::processButtonDownEvent(SDL_MouseButtonEvent & event)
{
	KeyDownEvent ev;
	ev.cursor_position = cursorPosition;
	ev.key = std::numeric_limits<uint64_t>::max() - event.button;
	if (engine->mrs->ui->processButtonDownEvent(ev))
		return;
	processKeyDown(ev.key);

	uint64_t action = keyBindings.getAction(ev.key);
	ev.key = action;
	if (engine->mrs->ui->processButtonDownEvent(ev))
		return;
	processKeyDown(action);
}

void InputSystem::processButtonUpEvent(SDL_MouseButtonEvent & event)
{
	KeyUpEvent ev;
	ev.cursor_position = cursorPosition;
	ev.key = std::numeric_limits<uint64_t>::max() - event.button;
	if (engine->mrs->ui->processButtonUpEvent(ev))
		return;
	processKeyUp(ev.key);

	uint64_t action = keyBindings.getAction(ev.key);
	ev.key = action;
	if (engine->mrs->ui->processButtonUpEvent(ev))
		return;
	processKeyUp(action);
}

void InputSystem::processTextInputEvent(SDL_TextInputEvent & event)
{
	std::string text(event.text);
	TextInputEvent ev;
	ev.cursor_position = cursorPosition;
	ev.text = text;
	if (engine->mrs->ui->processTextInputEvent(ev))
		return;
	for (auto& func : onTextInput)
		func(text);
}

void InputSystem::processMouseStayEvent()
{
	CursorMoveEvent ev;
	ev.cursor_index = 0;
	ev.cursor_position = cursorPosition;
	engine->mrs->ui->processCursorMoveEvent(ev);
}

void InputSystem::addKeyDownCallback(uint64_t key, const std::function<void(void)> & callback)
{
	onKeyDown.insert(std::make_pair(key, callback));
}

void InputSystem::addKeyUpCallback(uint64_t key, const std::function<void(void)> & callback)
{
	onKeyUp.insert(std::make_pair(key, callback));
}

void InputSystem::addUnfilteredKeyDownCallback(uint64_t key, const std::function<void(void)>& callback)
{
	onUnfilteredKeyDown.insert(std::make_pair(key, callback));
}

void InputSystem::addTextInputCallback(const std::function<void(const std::string&)> & callback)
{
	onTextInput.push_back(callback);
}

bool InputSystem::isKeyDown(uint64_t key)
{
	return keysDown.find(key) != keysDown.end();
}

bool InputSystem::isKeyPressed(uint64_t key)
{
	return keysPressed.find(key) != keysPressed.end();
}

bool InputSystem::isKeyReleased(uint64_t key)
{
	return keysReleased.find(key) != keysReleased.end();
}

Vec2 InputSystem::getCursor()
{
	return cursorPosition;
}

void InputSystem::tick(float dt)
{
	keysPressed.clear();
	keysReleased.clear();
}

void InputSystem::processKeyDown(uint64_t sym)
{
	if (sym == std::numeric_limits<uint32_t>::max())
		return;
	auto range = onKeyDown.equal_range(sym);
	for (auto i = range.first; i != range.second;)
	{
		auto& callback = i->second;
		if (callback)
		{
			callback();
			++i;
		}
		else
			onKeyDown.erase(i);
	}
	keysDown.insert(sym);
	keysPressed.insert(sym);
}

void InputSystem::processKeyUp(uint64_t sym)
{
	if (sym == std::numeric_limits<uint32_t>::max())
		return;
	auto range = onKeyUp.equal_range(sym);
	for (auto i = range.first; i != range.second;)
	{
		auto& callback = i->second;
		if (callback)
		{
			callback();
			++i;
		}
		else
			onKeyUp.erase(i);
	}
	keysDown.erase(sym);
	keysReleased.insert(sym);
}

void InputSystem::processUnfilteredKeyDown(uint64_t sym)
{
	if (sym == std::numeric_limits<uint32_t>::max())
		return;
	auto range = onUnfilteredKeyDown.equal_range(sym);
	for (auto i = range.first; i != range.second;)
	{
		auto& callback = i->second;
		if (callback)
		{
			callback();
			++i;
		}
		else
			onUnfilteredKeyDown.erase(i);
	}
}
