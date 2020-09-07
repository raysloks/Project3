#pragma once

#include <vector>

#include "Entity.h"

#include "SpriteRenderSystem.h"
#include "InputSystem.h"
#include "CustomBehaviourSystem.h"
#include "CollisionSystem.h"

class MobPosHandler;

#include "Level.h"

// forward declarations to avoid unnecessary includes

struct SDL_Window;
struct SDL_Surface;
struct SDL_Renderer;
struct SDL_Cursor;

class System;

class Sprite;

class SpriteSheet;

class Engine
{
public:
	Engine();
	~Engine();

	void run();

	void stop();

	SpriteRenderSystem * srs;
	InputSystem * input;
	CustomBehaviourSystem * cbs;
	CollisionSystem * cs;
	MobPosHandler * net;

	Level * level;

	double full;

	void setCursor(const std::shared_ptr<SpriteSheet> & sheet, int hotspot_x, int hotspot_y);

	void setLevel(Level * level);

private:

	SDL_Window * window;
	SDL_Renderer * render;

	bool stopped;

	double framerate_cap;
	float max_dt;

	std::vector<System*> systems;

	std::function<void(void)> fullscreen_toggle_func;

	void updateCursor();

	std::shared_ptr<SpriteSheet> cursor_sheet, cursor_sheet_new;
	int cursor_hotspot_x, cursor_hotspot_y;
	SDL_Cursor * cursor;

	void updateLevel();

	Level * level_new;

	void updateConveniencePointers();
};