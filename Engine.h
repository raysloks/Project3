#pragma once

#include <vector>

#include "Entity.h"

#include "InputSystem.h"
#include "CustomBehaviourSystem.h"

// forward declarations to avoid unnecessary includes

struct SDL_Window;
struct SDL_Surface;
struct SDL_Renderer;

class System;

class Sprite;

class Engine
{
public:
	Engine();
	~Engine();

	void run();

	void stop();

	InputSystem * input;
	CustomBehaviourSystem * cbs;

private:

	SDL_Window * window;
	SDL_Renderer * render;

	bool stopped;

	double framerate_cap;

	std::vector<Entity> entities;
	std::vector<System*> systems;
};