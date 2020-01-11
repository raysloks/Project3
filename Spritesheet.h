#pragma once

#include "Resource.h"

struct SDL_Renderer;
struct SDL_Surface;
struct SDL_Texture;

class SpriteSheet :
	public Resource<SpriteSheet>
{
public:
	SpriteSheet();
	SpriteSheet(size_t w, size_t h);
	~SpriteSheet();

	SpriteSheet(const SpriteSheet &) = delete;
	SpriteSheet(SpriteSheet &&) = delete;

	static std::shared_ptr<SpriteSheet> load(const std::string& fname);

	SDL_Texture * getTexture(SDL_Renderer * render);

	SDL_Surface * surface;
	SDL_Texture * texture;

	size_t rows, columns;

	int offset_x, offset_y;

	std::shared_ptr<SpriteSheet> makeCopy() const;

	std::shared_ptr<SpriteSheet> makeIsometricFloorLossy(bool blur) const;
	std::shared_ptr<SpriteSheet> makeIsometricFloorLossless(float rotation = 0.0f) const;

	std::shared_ptr<SpriteSheet> makeScaled(intmax_t scale) const;

	std::shared_ptr<SpriteSheet> makeOutline() const;
};

