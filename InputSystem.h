#pragma once

#include "System.h"

#include <map>

#include <functional>

#include <memory>

#include <set>

struct SDL_KeyboardEvent;

class SpriteRenderSystem;

class InputSystem :
	public System
{
public:
	void processKeyDownEvent(SDL_KeyboardEvent& event);
	void processKeyUpEvent(SDL_KeyboardEvent& event);

	void addKeyDownCallback(uint64_t key, const std::shared_ptr<std::function<void(void)>> & callback);
	void addKeyUpCallback(uint64_t key, const std::shared_ptr<std::function<void(void)>> & callback);

	bool isKeyDown(uint64_t key);
	bool isKeyPressed(uint64_t key);
	bool isKeyReleased(uint64_t key);

	Vec2 getCursorPosition();
	Vec2 getCursorPositionInWorld(SpriteRenderSystem * srs);

	void setKeyBinding(uint64_t action, uint64_t key);

	// clears pressed and released keys
	void tick(float dt);

private:

	Vec2 cursorPosition;

	std::map<uint64_t, uint64_t> keyBindings;

	void processKeyDown(uint64_t sym);
	void processKeyUp(uint64_t sym);

	std::set<uint64_t> keysDown, keysPressed, keysReleased;
	std::multimap<uint64_t, std::weak_ptr<std::function<void(void)>>> onKeyDown, onKeyUp;
};

