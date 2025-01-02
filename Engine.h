#pragma once

#include <vector>

#include "Entity.h"

#include "SpriteRenderSystem.h"
#include "InputSystem.h"
#include "CustomBehaviourSystem.h"
#include "CollisionSystem.h"

class MobPosHandler;
class ModelRenderSystem;

#include "Level.h"

// forward declarations to avoid unnecessary includes

struct SDL_Window;
struct SDL_Surface;
struct SDL_Renderer;
struct SDL_Cursor;

class System;

class Sprite;

class SpriteSheet;

class Window;

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
	ModelRenderSystem * mrs;

	Level * level;

	double full;

	void setCursor(const std::shared_ptr<SpriteSheet>& sheet);
	void setCursor(const std::shared_ptr<SpriteSheet>& sheet, int hotspot_x, int hotspot_y);
	void setCursor(SDL_SystemCursor id);

	void setLevel(Level * level);

private:

	SDL_Window * window;
	SDL_Renderer * render;

	bool stopped;

	double framerate_cap_fg, framerate_cap_bg;
	float max_dt;

	int profiling_counter, profiling_frame_window;
	double profiling_accumulator, profiling_low, profiling_high;

	std::vector<System*> systems;

	std::function<void(void)> fullscreen_toggle_func;

	void updateCursor();

	std::shared_ptr<SpriteSheet> cursor_sheet;
	SDL_SystemCursor cursor_id;
	int cursor_hotspot_x, cursor_hotspot_y;
	SDL_Cursor * cursor;
	bool cursor_dirty;

	void updateLevel();

	Level * level_new;

	void updateConveniencePointers();
};