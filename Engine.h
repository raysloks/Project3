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

class System;

class Sprite;

class Spritesheet;

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

	void add_entity(Entity&& entity);
	void remove_entity(Entity * entity);

//private:

	SDL_Window * window;
	SDL_Renderer * render;

	bool stopped;

	double framerate_cap;

	std::vector<Entity> entities;
	std::vector<System*> systems;

	std::shared_ptr<std::function<void(void)>> fullscreen_toggle_func;
};