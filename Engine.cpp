#include "Engine.h"

#include <SDL.h>
#include <SDL_image.h>

#include <iostream>

#include "SpriteRenderSystem.h"
#include "CollisionSystem.h"

#include "Player.h"

Engine::Engine()
{
	SDL_Init(SDL_INIT_VIDEO);

	IMG_Init(IMG_INIT_PNG);

	window = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	
	render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor(render, 255, 255, 255, 255);

	stopped = false;

	framerate_cap = 0.0;

	entities.reserve(1024);

	// initialize systems
	SpriteRenderSystem * srs = new SpriteRenderSystem(render);
	systems.push_back(srs);

	input = new InputSystem();
	systems.push_back(input);

	cbs = new CustomBehaviourSystem();
	systems.push_back(cbs);

	CollisionSystem * cs = new CollisionSystem();
	systems.push_back(cs);

	// create player
	{
		Entity entity;

		Sprite sprite;
		sprite.texture = SDL_CreateTextureFromSurface(render, IMG_Load("potato.png"));
		sprite.rect.x = 0;
		sprite.rect.y = 0;
		sprite.rect.w = 128;
		sprite.rect.h = 128;
		entity.addComponent(srs->sprites.add(std::move(sprite)));

		auto player = std::make_shared<Player>();
		player->srs = srs;
		entity.addComponent(&**cbs->behaviours.add(player));

		Collider collider;
		collider.r = 32.0f;
		entity.addComponent(cs->colliders.add(std::move(collider)));

		entities.emplace_back(std::move(entity));
	}

	auto texture = SDL_CreateTextureFromSurface(render, SDL_LoadBMP("test.bmp"));

	// create some props
	for (size_t i = 0; i < 100; ++i)
	{
		Entity entity;
		entity.p.x = i * 100;
		entity.p.y = 0;

		Sprite sprite;
		sprite.texture = texture;
		sprite.rect.x = 0;
		sprite.rect.y = 0;
		sprite.rect.w = 32;
		sprite.rect.h = 32;
		entity.addComponent(srs->sprites.add(std::move(sprite)));

		Collider collider;
		collider.r = 16.0f;
		entity.addComponent(cs->colliders.add(std::move(collider)));

		entities.emplace_back(std::move(entity));
	}
}

Engine::~Engine()
{
	SDL_DestroyRenderer(render);

	SDL_DestroyWindow(window);

	IMG_Quit();

	SDL_Quit();
}

void Engine::run()
{
	stopped = false;

	uint64_t freq = SDL_GetPerformanceFrequency();
	uint64_t start = SDL_GetPerformanceCounter();
	uint64_t start_busy = SDL_GetPerformanceCounter();
	uint64_t end, end_busy;

	double full = 1.0 / 60.0;
	double busy = 1.0 / 60.0;

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

		SDL_SetRenderDrawColor(render, 100, 200, 100, 255);
		SDL_RenderClear(render);

		for (auto system : systems)
			system->tick(full);

		end_busy = SDL_GetPerformanceCounter();
		busy = double(end_busy - start_busy) / freq;
		if (framerate_cap > 0.0)
		{
			double delay = (1.0 / framerate_cap - busy) * 1000.0;
			if (delay > 0)
				SDL_Delay(delay);
		}
		start_busy = SDL_GetPerformanceCounter();

		end = SDL_GetPerformanceCounter();
		full = double(end - start) / freq;
		start = SDL_GetPerformanceCounter();

		std::cout << 1.0 / full << std::endl;

		SDL_RenderPresent(render);
	}
}

void Engine::stop()
{
	stopped = true;
}
