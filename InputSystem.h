#pragma once

#include "System.h"

#include <map>

#include <functional>

#include <memory>

#include <set>

struct SDL_KeyboardEvent;

class InputSystem :
	public System
{
public:
	void processKeyDown(SDL_KeyboardEvent& event);
	void processKeyUp(SDL_KeyboardEvent& event);

	void addKeyDownCallback(int key, const std::shared_ptr<std::function<void(void)>> & callback);
	void addKeyUpCallback(int key, const std::shared_ptr<std::function<void(void)>> & callback);

	bool isKeyDown(int key);

private:

	std::set<int> keysDown;
	std::multimap<int, std::weak_ptr<std::function<void(void)>>> onKeyDown, onKeyUp;
};

