#include "InputSystem.h"

#include <SDL.h>

void InputSystem::processKeyDownEvent(SDL_KeyboardEvent & event)
{
	// ignore auto-repeated key down events
	if (event.repeat)
		return;
	processKeyDown(event.keysym.sym);
	auto i = keyBindings.find(event.keysym.sym);
	if (i != keyBindings.end())
		processKeyDown(i->second);
}

void InputSystem::processKeyUpEvent(SDL_KeyboardEvent & event)
{
	processKeyUp(event.keysym.sym);
	auto i = keyBindings.find(event.keysym.sym);
	if (i != keyBindings.end())
		processKeyUp(i->second);
}

void InputSystem::addKeyDownCallback(uint64_t key, const std::function<void(void)> & callback)
{
	onKeyDown.insert(std::make_pair(key, callback));
}

void InputSystem::addKeyUpCallback(uint64_t key, const std::function<void(void)> & callback)
{
	onKeyUp.insert(std::make_pair(key, callback));
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

Vec2 InputSystem::getCursorPosition()
{
	return cursorPosition;
}

Vec2 InputSystem::getCursorPositionInWorld(SpriteRenderSystem * srs)
{
	return Vec2();
}

void InputSystem::setKeyBinding(uint64_t action, uint64_t key)
{
	keyBindings[key] = action;
}

void InputSystem::tick(float dt)
{
	keysPressed.clear();
	keysReleased.clear();
}

void InputSystem::processKeyDown(uint64_t sym)
{
	auto range = onKeyDown.equal_range(sym);
	for (auto i = range.first; i != range.second;)
	{
		auto callback = i->second;
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
	auto range = onKeyUp.equal_range(sym);
	for (auto i = range.first; i != range.second;)
	{
		auto callback = i->second;
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
