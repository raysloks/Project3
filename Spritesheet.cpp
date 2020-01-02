#include "Spritesheet.h"

#include <SDL.h>
#include <SDL_image.h>

#include <thread>

Spritesheet::Spritesheet(const std::string& fname)
{
	surface = nullptr;
	texture = nullptr;

	rows = 1;
	columns = 1;

	loaded = false;

	std::thread t([this, fname]()
		{
			surface = IMG_Load(fname.c_str());
			loaded = true;
		});
	t.detach();
}

SDL_Texture * Spritesheet::getTexture(SDL_Renderer * render)
{
	if (!texture)
		if (surface)
			texture = SDL_CreateTextureFromSurface(render, surface);
	return texture;
}
