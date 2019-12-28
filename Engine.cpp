#include "Engine.h"

#include <SDL.h>

#include <iostream>

#include "SpriteRenderSystem.h"

#include "Player.h"

Engine::Engine()
{
	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	
	render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor(render, 0xff, 0xff, 0xff, 0xff);

	stopped = false;

	entities.reserve(1024);

	// initialize systems
	SpriteRenderSystem * sr = new SpriteRenderSystem(nullptr, render);

	systems.push_back(sr);

	input = new InputSystem();

	systems.push_back(input);

	cbs = new CustomBehaviourSystem();

	systems.push_back(cbs);



	// create player

	entities.emplace_back(Entity());

	entities.at(0).p.x = 128;

	Sprite sprite;
	sprite.texture = SDL_CreateTextureFromSurface(render, SDL_LoadBMP("test.bmp"));
	sprite.rect.x = 0;
	sprite.rect.y = 0;
	sprite.rect.w = 64;
	sprite.rect.h = 64;
	sprite.entity = &entities.at(0);

	sr->sprites.add(std::move(sprite));

	auto player = std::make_shared<Player>();
	player->entity = &entities.at(0);

	cbs->behaviours.add(player);
}

Engine::~Engine()
{
	SDL_DestroyRenderer(render);

	SDL_DestroyWindow(window);

	SDL_Quit();
}

void Engine::run()
{
	stopped = false;

	uint64_t freq = SDL_GetPerformanceFrequency();
	uint64_t start = SDL_GetPerformanceCounter();
	uint64_t end;
	double duration = 1.0 / 60.0;

	while (!stopped)
	{
		SDL_Event e;

		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				stop();
				break;
			case SDL_KEYDOWN:
				input->processKeyDown(e.key);
				break;
			case SDL_KEYUP:
				input->processKeyUp(e.key);
				break;
			case SDL_WINDOWEVENT:
				switch (e.window.event)
				{
				case SDL_WINDOWEVENT_RESIZED:
					//screen = SDL_GetWindowSurface(window);
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
		}

		SDL_RenderClear(render);

		for (auto system : systems)
			system->tick(duration);

		end = SDL_GetPerformanceCounter();
		duration = double(end - start) / freq;
		start = SDL_GetPerformanceCounter();

		std::cout << 1.0 / duration << std::endl;

		SDL_RenderPresent(render);

		
	}
}

void Engine::stop()
{
	stopped = true;
}
