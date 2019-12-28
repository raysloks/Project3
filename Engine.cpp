#include "Engine.h"

#include <SDL.h>

#include <iostream>

#include "SpriteRender.h"

Engine::Engine()
{
	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);

	screen = SDL_GetWindowSurface(window);

	stopped = false;

	entities.reserve(1024);
	entities.emplace_back(Entity());

	entities.at(0).p.x = 128;

	SpriteRender * sr = new SpriteRender(nullptr, screen);

	Sprite sprite;
	sprite.surface = SDL_LoadBMP("test.bmp");
	sprite.rect.x = 0;
	sprite.rect.y = 0;
	sprite.rect.w = 64;
	sprite.rect.h = 64;
	sprite.entity = &entities.at(0);

	sr->sprites.add(std::move(sprite));

	systems.push_back(sr);
}

Engine::~Engine()
{
	SDL_DestroyWindow(window);

	SDL_Quit();
}

void Engine::run()
{
	stopped = false;

	while (!stopped)
	{
		SDL_Event e;

		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
				stop();

			if (e.type == SDL_KEYDOWN)
			{
				std::cout << SDL_GetKeyName(e.key.keysym.sym);
			}
		}

		for (auto system : systems)
			system->tick(1.0f / 60.0f);

		SDL_UpdateWindowSurface(window);
	}
}

void Engine::stop()
{
	stopped = true;
}
