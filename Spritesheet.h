#pragma once

#include "Resource.h"

struct SDL_Renderer;
struct SDL_Surface;
struct SDL_Texture;

class Spritesheet :
	public Resource<Spritesheet>
{
public:
	Spritesheet(const std::string& fname);

	SDL_Texture * getTexture(SDL_Renderer * render);

	SDL_Surface * surface;
	SDL_Texture * texture;
};

