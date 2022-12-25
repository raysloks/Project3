#include "Engine.h"

#include <SDL.h>
#include <SDL_image.h>

#include <iostream>

#include "CustomBehaviour.h"

#include "MobPosHandler.h"
#include "ModelRenderSystem.h"

Engine::Engine()
{
	SDL_Init(SDL_INIT_VIDEO);

	IMG_Init(IMG_INIT_PNG);

	window = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_VULKAN);
	
	/*render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor(render, 255, 255, 255, 255);*/

	stopped = false;

	framerate_cap_fg = 0.0;
	framerate_cap_bg = 30.0;

	max_dt = 1.0 / 30.0;


	//SDL_SetWindowGrab(window, SDL_TRUE);


	// create level instance

	level = new Level();
	level_new = level;


	// initialize systems

	mrs = new ModelRenderSystem();
	mrs->init(window);
	systems.push_back(mrs);

	srs = new SpriteRenderSystem(render, window);
	systems.push_back(srs);

	input = new InputSystem();
	systems.push_back(input);

	net = new MobPosHandler();
	systems.push_back(net);

	cbs = new CustomBehaviourSystem();
	systems.push_back(cbs);

	cs = new CollisionSystem();
	systems.push_back(cs);


	updateConveniencePointers();


	// add fullscreen toggle
	fullscreen_toggle_func = [this]()
	{
		if (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP)
			SDL_SetWindowFullscreen(window, 0);
		else
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	};
	input->addKeyDownCallback(SDLK_F11, fullscreen_toggle_func);


	cursor_sheet_new = nullptr;
	cursor_sheet = nullptr;
	cursor = nullptr;

	profiling_frame_window = 2000;
	profiling_counter = profiling_frame_window;
	profiling_accumulator = 0.0;
	profiling_low = std::numeric_limits<double>::infinity();
	profiling_high = 0.0;
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
		updateLevel();

		{
			std::lock_guard<std::mutex> lock(net->mutex);

			updateCursor();

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
				case SDL_MOUSEWHEEL:
					input->processMouseWheelEvent(e.wheel);
					break;
				case SDL_MOUSEBUTTONDOWN:
					input->processButtonDownEvent(e.button);
					break;
				case SDL_MOUSEBUTTONUP:
					input->processButtonUpEvent(e.button);
					break;
				case SDL_TEXTINPUT:
					input->processTextInputEvent(e.text);
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
		}

		SpriteSheet::destroyUnusedTextures();

		end_busy = SDL_GetPerformanceCounter();
		busy = double(end_busy - start_busy) / freq;
		double framerate_cap = SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS ? framerate_cap_fg : framerate_cap_bg;
		if (framerate_cap > 0.0)
		{
			double delay = (1.0 / framerate_cap - busy) * 1000.0;
			if (delay > 0)
				SDL_Delay(delay);
		}
		start_busy = SDL_GetPerformanceCounter();

		//SDL_RenderPresent(render);

		//end = SDL_GetPerformanceCounter();
		end = mrs->getPresentTime();
		uint64_t diff = end - start;
		start = end;

		full = double(diff) / freq;

		profiling_accumulator += full;
		profiling_low = fmin(profiling_low, full);
		profiling_high = fmax(profiling_high, full);

		if (--profiling_counter == 0)
		{
			std::cout << "avg: " << double(profiling_frame_window) / profiling_accumulator << " fps" << std::endl;
			std::cout << "min: " << 1.0 / profiling_high << " fps" << std::endl;
			std::cout << "max: " << 1.0 / profiling_low << " fps" << std::endl;

			profiling_counter = profiling_frame_window;
			profiling_accumulator = 0.0;
			profiling_low = std::numeric_limits<double>::infinity();
			profiling_high = 0.0;

			std::cout << double(mrs->wait_fences_a) / freq * 1000.0 << std::endl;
			std::cout << double(mrs->wait_fences_b) / freq * 1000.0 << std::endl;
			std::cout << double(mrs->wait_c) / freq * 1000.0 << std::endl;
			std::cout << double(mrs->wait_d) / freq * 1000.0 << std::endl;
		}
	}
}

void Engine::stop()
{
	stopped = true;
}

void Engine::setCursor(const std::shared_ptr<SpriteSheet>& sheet, int hotspot_x, int hotspot_y)
{
	cursor_sheet_new = sheet;
	cursor_hotspot_x = hotspot_x;
	cursor_hotspot_y = hotspot_y;
}

void Engine::setLevel(Level * level_new_new)
{
	level_new = level_new_new;
}

void Engine::updateCursor()
{
	if (cursor_sheet != cursor_sheet_new)
	{
		auto cursor_prev = cursor;
		if (cursor_sheet_new)
		{
			if (cursor_sheet_new->loaded)
			{
				if (cursor_sheet_new->surface)
					cursor = SDL_CreateColorCursor(cursor_sheet_new->surface, cursor_hotspot_x, cursor_hotspot_y);
				else
					cursor = nullptr; // i guess we just default the cursor if we failed to load the requested image
				cursor_sheet = cursor_sheet_new;
			}
		}
		else
		{
			cursor = nullptr;
			cursor_sheet = cursor_sheet_new;
		}

		if (cursor != cursor_prev)
		{
			SDL_SetCursor(cursor);
			if (cursor_prev)
				SDL_FreeCursor(cursor_prev);
		}
	}
}

void Engine::updateLevel()
{
	if (level != level_new)
	{
		level = level_new;

		updateConveniencePointers();
	}
}

void Engine::updateConveniencePointers()
{
	// allow custom behaviours easy access to systems
	CustomBehaviour::engine = this;

	CustomBehaviour::cs = cs;
	CustomBehaviour::input = input;
	CustomBehaviour::cbs = cbs;
	CustomBehaviour::srs = srs;
	CustomBehaviour::mrs = mrs;

	CustomBehaviour::level = level;
	CustomBehaviour::tm = &level->tilemap;
	CustomBehaviour::rng = &level->rng;

	CustomBehaviour::net = net;

	for (auto system : systems)
	{
		system->engine = this;
		system->level = level;
	}
}
