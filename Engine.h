#pragma once

#include <vector>

#include "Entity.h"

// forward declarations to avoid unnecessary includes

struct SDL_Window;
struct SDL_Surface;

class System;

class Sprite;

class Engine
{
public:
	Engine();
	~Engine();

	void run();

	void stop();

private:

	SDL_Window * window;
	SDL_Surface * screen;

	bool stopped;

	std::vector<Entity> entities;
	std::vector<System*> systems;
};