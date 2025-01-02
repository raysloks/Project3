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

	window = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1920, 1080, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_VULKAN);
	
	/*render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor(render, 255, 255, 255, 255);*/

	SDL_StopTextInput();

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


	cursor_sheet = nullptr;
	cursor_id = SDL_SYSTEM_CURSOR_ARROW;
	cursor = nullptr;

	profiling_frame_window = 500;
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

	uint64_t max_a = 0;
	uint64_t max_b = 0;
	uint64_t max_c = 0;
	uint64_t max_d = 0;
	uint64_t max_e = 0;
	uint64_t max_total = 0;
	uint64_t max_external = 0;

	std::vector<uint64_t> times(systems.size() + 1);
	std::vector<uint64_t> waits(systems.size());
	std::vector<uint64_t> max_waits(systems.size());
	std::vector<std::string> system_names = {
		"mrs",
		"srs",
		"input",
		"net",
		"cbs",
		"cs"
	};

	uint64_t profiling_frame_counter = 0;

	int64_t ufo_pos = 0;
	int64_t testgrid_pos = 0;

	while (!stopped)
	{
		updateLevel();

		{
			std::lock_guard<std::mutex> lock(net->mutex);

			//updateCursor();

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
				mrs->ui->updateCursor();
			}

			input->processMouseStayEvent();
			mrs->ui->updateCursor();

			times[0] = SDL_GetPerformanceCounter();
			for (size_t i = 0; i < systems.size(); ++i)
			{
				auto system = systems[i];
				//system->tick(1.0 / 240.0);
				system->tick(fmin(full, max_dt));
				times[i + 1] = SDL_GetPerformanceCounter();
			}

			for (size_t i = 0; i < waits.size(); ++i)
				waits[i] = times[i + 1] - times[i];
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

		if (profiling_counter < profiling_frame_window - 100)
		{
			++profiling_frame_counter;
			profiling_accumulator += full;
			profiling_low = fmin(profiling_low, full);
			profiling_high = fmax(profiling_high, full);

			uint64_t total = mrs->wait_a + mrs->wait_b + mrs->wait_c + mrs->wait_d + mrs->wait_e;
			if (total > max_total)
			{
				max_a = mrs->wait_a;
				max_b = mrs->wait_b;
				max_c = mrs->wait_c;
				max_d = mrs->wait_d;
				max_e = mrs->wait_e;
				max_total = total;
			}
			max_external = std::max(mrs->wait_external, max_external);
			for (size_t i = 0; i < max_waits.size(); ++i)
				max_waits[i] = std::max(waits[i], max_waits[i]);
		}

		//if (--profiling_counter == 0)
		//{
		//	std::cout << "avg: " << double(profiling_frame_counter) / profiling_accumulator << " fps\n";
		//	std::cout << "min: " << profiling_low * 1000.0 << " ms\n";
		//	std::cout << "max: " << profiling_high * 1000.0 << " ms\n";
		//	std::cout << "\n";

		//	profiling_frame_counter = 0;
		//	profiling_counter = profiling_frame_window;
		//	profiling_accumulator = 0.0;
		//	profiling_low = std::numeric_limits<double>::infinity();
		//	profiling_high = 0.0;

		//	/*std::cout << "a: " << double(mrs->wait_fences_a) / freq * 1000.0 << std::endl;
		//	std::cout << "b: " << double(mrs->wait_fences_b) / freq * 1000.0 << std::endl;
		//	std::cout << "c: " << double(mrs->wait_c) / freq * 1000.0 << std::endl;
		//	std::cout << "d: " << double(mrs->wait_d) / freq * 1000.0 << std::endl;
		//	std::cout << "e: " << double(mrs->wait_e) / freq * 1000.0 << std::endl;*/

		//	std::cout << "a: " << double(max_a) / freq * 1000.0 << "\n";
		//	std::cout << "b: " << double(max_b) / freq * 1000.0 << "\n";
		//	std::cout << "c: " << double(max_c) / freq * 1000.0 << "\n";
		//	std::cout << "d: " << double(max_d) / freq * 1000.0 << "\n";
		//	std::cout << "e: " << double(max_e) / freq * 1000.0 << "\n";
		//	std::cout << "total: " << double(max_total) / freq * 1000.0 << "\n\n";
		//	std::cout << "external: " << double(max_external) / freq * 1000.0 << "\n\n";

		//	for (size_t i = 0; i < max_waits.size(); ++i)
		//		std::cout << system_names[i] << ": " << double(max_waits[i]) / freq * 1000.0 << "\n";
		//	std::cout << std::endl;

		//	max_a = 0;
		//	max_b = 0;
		//	max_c = 0;
		//	max_d = 0;
		//	max_e = 0;
		//	max_total = 0;
		//	max_external = 0;
		//	for (size_t i = 0; i < max_waits.size(); ++i)
		//		max_waits[i] = 0;
		//}
	}
}

void Engine::stop()
{
	stopped = true;
}

void Engine::setCursor(const std::shared_ptr<SpriteSheet>& sheet)
{
	setCursor(sheet, sheet->offset_x, sheet->offset_y);
}

void Engine::setCursor(const std::shared_ptr<SpriteSheet>& sheet, int hotspot_x, int hotspot_y)
{
	if (sheet != cursor_sheet || hotspot_x != cursor_hotspot_x || hotspot_y != cursor_hotspot_y)
		cursor_dirty = true;
	cursor_sheet = sheet;
	cursor_hotspot_x = hotspot_x;
	cursor_hotspot_y = hotspot_y;
	cursor_id = SDL_SYSTEM_CURSOR_ARROW;

	updateCursor();
}

void Engine::setCursor(SDL_SystemCursor id)
{
	if (id != cursor_id || cursor_sheet != nullptr)
		cursor_dirty = true;
	cursor_id = id;
	cursor_sheet = nullptr;

	updateCursor();
}

void Engine::setLevel(Level * level_new_new)
{
	level_new = level_new_new;
}

void Engine::updateCursor()
{
	if (cursor_dirty)
	{
		if (cursor)
			SDL_FreeCursor(cursor);
		if (cursor_sheet != nullptr)
		{
			if (cursor_sheet->loaded)
			{
				if (cursor_sheet->surface)
					cursor = SDL_CreateColorCursor(cursor_sheet->surface, cursor_hotspot_x, cursor_hotspot_y);
				else
					cursor = nullptr; // i guess we just default the cursor if we failed to load the requested image
			}
		}
		else
		{
			cursor = SDL_CreateSystemCursor(cursor_id);
		}
		SDL_SetCursor(cursor);
		cursor_dirty = false;
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
