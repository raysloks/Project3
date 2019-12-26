#include "Engine.h"

Engine::Engine()
{
	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);

	screen = SDL_GetWindowSurface(window);
}

Engine::~Engine()
{
	SDL_DestroyWindow(window);

	SDL_Quit();
}
