#include "Engine.h"

#include <SDL.h>
#include <SDL_image.h>

#include <iostream>

#include "CustomBehaviour.h"

Engine::Engine()
{
	SDL_Init(SDL_INIT_VIDEO);

	IMG_Init(IMG_INIT_PNG);

	window = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	
	render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor(render, 255, 255, 255, 255);

	stopped = false;

	framerate_cap = 0.0;

	max_dt = 1.0 / 30.0;

	// initialize systems

	cbs = new CustomBehaviourSystem();
	systems.push_back(cbs);

	cs = new CollisionSystem();
	systems.push_back(cs);

	srs = new SpriteRenderSystem(render);
	systems.push_back(srs);

	input = new InputSystem();
	systems.push_back(input);


	// allow custom behaviours easy access to systems
	CustomBehaviour::engine = this;
	CustomBehaviour::cs = cs;
	CustomBehaviour::input = input;
	CustomBehaviour::cbs = cbs;
	CustomBehaviour::srs = srs;


	// add fullscreen toggle
	fullscreen_toggle_func = [this]()
	{
		if (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP)
			SDL_SetWindowFullscreen(window, 0);
		else
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	};
	input->addKeyDownCallback(SDLK_F11, fullscreen_toggle_func);
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

	full = 1.0f / 60.0;
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
				input->processKeyDownEvent(e.key);
				break;
			case SDL_KEYUP:
				input->processKeyUpEvent(e.key);
				break;
			case SDL_MOUSEMOTION:
				input->processMouseMoveEvent(e.motion);
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

		for (auto system : systems)
		{
			//auto start = SDL_GetPerformanceCounter();
			system->tick(fmin(full, max_dt));
			/*auto end = SDL_GetPerformanceCounter();
			double duration = double(end - start) / freq;
			std::cout << typeid(*system).name() << std::endl;
			std::cout << 100 * duration / full << std::endl;*/
		}

		end_busy = SDL_GetPerformanceCounter();
		busy = double(end_busy - start_busy) / freq;
		if (framerate_cap > 0.0)
		{
			double delay = (1.0 / framerate_cap - busy) * 1000.0;
			if (delay > 0)
				SDL_Delay(delay);
		}
		start_busy = SDL_GetPerformanceCounter();

		SDL_RenderPresent(render);

		end = SDL_GetPerformanceCounter();
		uint64_t diff = end - start;
		start = end;

		full = double(diff) / freq;
	}
}

void Engine::stop()
{
	stopped = true;
}

size_t Engine::add_entity(Entity && entity)
{
	return entities.add(std::move(entity)).index;
}

Entity * Engine::get_entity(size_t index)
{
	if (index > entities.components.size())
		return nullptr;
	return &entities.components[index];
}

void Engine::remove_entity(Entity * entity)
{
	*entity = std::move(Entity());
	entities.remove(entity);
}

void Engine::remove_entity(size_t entity)
{
	remove_entity(&entities.components[entity]);
}
