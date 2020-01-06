#include "SpriteSheet.h"

#include <SDL.h>
#include <SDL_image.h>

#include <thread>

SpriteSheet::SpriteSheet(size_t w, size_t h)
{
	surface = SDL_CreateRGBSurface(0, w, h, 32, 0xff, 0xff << 8, 0xff << 16, 0xff << 24);
	texture = nullptr;

	rows = 1;
	columns = 1;

	offset_x = 0;
	offset_y = 0;

	loaded = true;
}

SpriteSheet::SpriteSheet(const std::string& fname)
{
	surface = nullptr;
	texture = nullptr;

	rows = 1;
	columns = 1;

	offset_x = 0;
	offset_y = 0;

	loaded = false;

	std::thread t([this, fname]()
		{
			surface = IMG_Load(fname.c_str());
			loaded = true;
		});
	t.detach();
}

SpriteSheet::~SpriteSheet()
{
	if (surface)
		SDL_FreeSurface(surface);
	if (texture)
		SDL_DestroyTexture(texture);
}

SDL_Texture * SpriteSheet::getTexture(SDL_Renderer * render)
{
	if (!texture)
		if (surface)
			texture = SDL_CreateTextureFromSurface(render, surface);
	return texture;
}
