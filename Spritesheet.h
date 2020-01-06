#pragma once

#include "Resource.h"

struct SDL_Renderer;
struct SDL_Surface;
struct SDL_Texture;

class SpriteSheet :
	public Resource<SpriteSheet>
{
public:
	SpriteSheet(size_t w, size_t h);
	SpriteSheet(const std::string& fname);
	~SpriteSheet();

	SDL_Texture * getTexture(SDL_Renderer * render);

	SDL_Surface * surface;
	SDL_Texture * texture;

	size_t rows, columns;

	int offset_x, offset_y;
};

