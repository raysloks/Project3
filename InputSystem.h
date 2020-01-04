#pragma once

#include "System.h"

#include <map>

#include <functional>

#include <memory>

#include <set>

#include "KeyBindings.h"

struct SDL_KeyboardEvent;
struct SDL_MouseMotionEvent;

class SpriteRenderSystem;

class InputSystem :
	public System
{
public:
	void processKeyDownEvent(SDL_KeyboardEvent& event);
	void processKeyUpEvent(SDL_KeyboardEvent& event);

	void processMouseMoveEvent(SDL_MouseMotionEvent& event);

	void addKeyDownCallback(uint64_t key, const std::function<void(void)> & callback);
	void addKeyUpCallback(uint64_t key, const std::function<void(void)> & callback);

	bool isKeyDown(uint64_t key);
	bool isKeyPressed(uint64_t key);
	bool isKeyReleased(uint64_t key);

	Vec2 getCursor();

	// clears pressed and released keys
	void tick(float dt);

	KeyBindings keyBindings;

private:

	Vec2 cursorPosition;

	void processKeyDown(uint64_t sym);
	void processKeyUp(uint64_t sym);

	std::set<uint64_t> keysDown, keysPressed, keysReleased;
	std::multimap<uint64_t, std::function<void(void)>> onKeyDown, onKeyUp;
};

