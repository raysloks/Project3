#pragma once

#include "System.h"

#include <map>

#include <functional>

#include <memory>

#include <set>

#include "KeyBindings.h"

struct SDL_KeyboardEvent;
struct SDL_MouseMotionEvent;
struct SDL_MouseWheelEvent;
struct SDL_MouseButtonEvent;
struct SDL_TextInputEvent;

class InputSystem :
	public System
{
public:
	void processKeyDownEvent(SDL_KeyboardEvent& event);
	void processKeyUpEvent(SDL_KeyboardEvent& event);

	void processMouseMoveEvent(SDL_MouseMotionEvent& event);
	void processMouseWheelEvent(SDL_MouseWheelEvent& event);

	void processButtonDownEvent(SDL_MouseButtonEvent& event);
	void processButtonUpEvent(SDL_MouseButtonEvent& event);

	void processTextInputEvent(SDL_TextInputEvent& event);

	void addKeyDownCallback(uint64_t key, const std::function<void(void)> & callback);
	void addKeyUpCallback(uint64_t key, const std::function<void(void)> & callback);
	void addUnfilteredKeyDownCallback(uint64_t key, const std::function<void(void)> & callback);

	void addTextInputCallback(const std::function<void(const std::string&)> & callback);

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
	void processUnfilteredKeyDown(uint64_t sym);

	std::set<uint64_t> keysDown, keysPressed, keysReleased;
	std::multimap<uint64_t, std::function<void(void)>> onKeyDown, onKeyUp, onUnfilteredKeyDown;

	std::vector<std::function<void(const std::string&)>> onTextInput;
};

