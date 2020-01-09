#pragma once

#include <vector>

#include "Entity.h"

#include "SpriteRenderSystem.h"
#include "InputSystem.h"
#include "CustomBehaviourSystem.h"
#include "CollisionSystem.h"

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

	Reference<Entity> add_entity(Entity&& entity);
	Entity * get_entity(size_t index);
	void remove_entity(Entity * entity);
	void remove_entity(size_t index);

	double full;

	void setCursor(const std::shared_ptr<SpriteSheet> & sheet);

private:

	SDL_Window * window;
	SDL_Renderer * render;

	bool stopped;

	double framerate_cap;
	float max_dt;

	ComponentContainer<Entity> entities;
	std::vector<System*> systems;

	std::function<void(void)> fullscreen_toggle_func;

	std::shared_ptr<SpriteSheet> cursor_sheet, cursor_sheet_new;
	SDL_Cursor * cursor;
};