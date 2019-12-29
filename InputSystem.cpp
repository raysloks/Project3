#include "InputSystem.h"

#include <SDL.h>

void InputSystem::processKeyDown(SDL_KeyboardEvent & event)
{
	if (event.repeat)
		return;
	auto range = onKeyDown.equal_range(event.keysym.sym);
	for (auto i = range.first; i != range.second;)
	{
		auto callback = i->second.lock();
		if (callback)
		{
			(*callback)();
			++i;
		}
		else
			onKeyDown.erase(i);
	}
	keysDown.insert(event.keysym.sym);
}

void InputSystem::processKeyUp(SDL_KeyboardEvent & event)
{
	auto range = onKeyUp.equal_range(event.keysym.sym);
	for (auto i = range.first; i != range.second;)
	{
		auto callback = i->second.lock();
		if (callback)
		{
			(*callback)();
			++i;
		}
		else
			onKeyUp.erase(i);
	}
	keysDown.erase(event.keysym.sym);
}

void InputSystem::addKeyDownCallback(int key, const std::shared_ptr<std::function<void(void)>>& callback)
{
	onKeyDown.insert(std::make_pair(key, callback));
}

void InputSystem::addKeyUpCallback(int key, const std::shared_ptr<std::function<void(void)>>& callback)
{
	onKeyUp.insert(std::make_pair(key, callback));
}

bool InputSystem::isKeyDown(int key)
{
	return keysDown.find(key) != keysDown.end();
}
