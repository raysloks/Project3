#include "NoiseNode.h"

#include "Xoroshiro128Plus.h"

void NoiseNode::assemble()
{
	auto sheet = std::make_shared<SpriteSheet>(width, height);
	result.result = sheet;

	Xoroshiro128Plus rng(seed0, seed1);

	SDL_Surface * surface = SDL_CreateRGBSurface(0, width, height, 32, 0xff, 0xff << 8, 0xff << 16, 0xff << 24);
	intmax_t pitch = surface->pitch;
	uint8_t * pixels = (uint8_t*)surface->pixels;

	sheet->surface = surface;

	for (intmax_t y = 0; y < height; ++y)
	{
		for (intmax_t x = 0; x < width; ++x)
		{
			SDL_Color& color = *(SDL_Color*)(pixels + y * pitch + x * 4);
			uint64_t value = rng.next();
			color.r = value >> 8 & 0xff;
			color.g = value >> 16 & 0xff;
			color.b = value >> 24 & 0xff;
			color.a = 255;
		}
	}

	sheet->loaded = true;
}
