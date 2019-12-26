#pragma once

#include <SDL.h>

class Engine
{
public:
	Engine();
	~Engine();

private:

	SDL_Window * window;
	SDL_Surface * screen;
};