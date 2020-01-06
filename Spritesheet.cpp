#include "SpriteSheet.h"

#include <SDL.h>
#include <SDL_image.h>

#include <thread>

#include "Vec2.h"

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

SpriteSheet::SpriteSheet(const std::string & fname)
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

SpriteSheet::SpriteSheet(const SpriteSheet & sheet)
{
	surface = nullptr;
	texture = nullptr;

	rows = sheet.rows;
	columns = sheet.columns;

	offset_x = sheet.offset_x;
	offset_y = sheet.offset_y;

	if (sheet.surface)
	{
		auto surface = sheet.surface;
		auto format = sheet.surface->format;
		this->surface = SDL_CreateRGBSurfaceFrom(surface->pixels, surface->w, surface->h, surface->pitch, format->BitsPerPixel, format->Rmask, format->Gmask, format->Bmask, format->Amask);

		loaded = true;
	}

	// TODO on load
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

std::shared_ptr<SpriteSheet> SpriteSheet::makeIsometricFloorLossy(bool blur)
{
	while (!loaded)
	{
		SDL_Delay(0); // TODO on load
	}
	if (surface)
	{
		intmax_t w = surface->w;
		intmax_t h = surface->h;
		intmax_t nw = w + h;
		intmax_t nh = nw / 2;

		auto sheet = std::make_shared<SpriteSheet>(nw, nh);
		sheet->columns = columns;
		sheet->rows = rows;

		for (intmax_t x = 0; x < nw; ++x)
		{
			for (intmax_t y = 0; y < nh; ++y)
			{
				intmax_t ox = (x + 1) / 2 + y + h / 2 - nh;
				intmax_t oy = y - (x + 1) / 2 + h / 2;

				SDL_Color & iso = *(SDL_Color*)((uint8_t*)sheet->surface->pixels + y * sheet->surface->pitch + x * 4);

				if (ox >= 0 && ox < w && oy >= 0 && oy < h)
				{
					SDL_Color & og = *(SDL_Color*)((uint8_t*)surface->pixels + oy * surface->pitch + ox * 4);

					iso = og;

					if (blur)
					{
						if (ox % 2)
						{
							oy -= 1;
						}
						else
						{
							oy += 1;
						}

						SDL_Color & og = *(SDL_Color*)((uint8_t*)surface->pixels + oy * surface->pitch + ox * 4);

						iso.r = (iso.r + og.r) / 2;
						iso.g = (iso.g + og.g) / 2;
						iso.b = (iso.b + og.b) / 2;
						iso.a = (iso.a + og.a) / 2;
					}
				}
				else
				{
					iso.r = 0;
					iso.g = 0;
					iso.b = 0;
					iso.a = 0;
				}
			}
		}

		return sheet;
	}
	return nullptr;
}

std::shared_ptr<SpriteSheet> SpriteSheet::makeIsometricFloorLossless(float rotation)
{
	while (!loaded)
	{
		SDL_Delay(0); // TODO on load
	}
	if (surface)
	{
		intmax_t wf = surface->w;
		intmax_t hf = surface->h;

		intmax_t w = wf / columns;
		intmax_t h = hf / rows;
		intmax_t nw = w + h;
		intmax_t nh = nw / 2;

		intmax_t nwf = nw * columns;
		intmax_t nhf = nh * rows;

		auto sheet = std::make_shared<SpriteSheet>(nwf, nhf);

		sheet->columns = columns;
		sheet->rows = rows;

		for (intmax_t c = 0; c < columns; ++c)
		{
			for (intmax_t r = 0; r < rows; ++r)
			{
				for (intmax_t x = 0; x < nw; ++x)
				{
					for (intmax_t y = 0; y < nh; ++y)
					{
						intmax_t xrfx = x;
						intmax_t xrfy = x;

						intmax_t yr = y;

						xrfy += 1;

						intmax_t ox = xrfx / 2 + y + h / 2 - nh;
						intmax_t oy = y - xrfy / 2 + h / 2;

						if (rotation != 0.0f)
						{
							Vec2 temp(ox - w / 2, oy - h / 2);
							temp.Rotate(rotation);

							ox = lroundf(temp.x + w / 2);
							oy = lroundf(temp.y + h / 2);
						}

						SDL_Color & iso = *(SDL_Color*)((uint8_t*)sheet->surface->pixels + (y + r * nh) * sheet->surface->pitch + (x + c * nw) * 4);

						if (ox >= 0 && ox < w && oy >= 0 && oy < h)
						{
							SDL_Color & og = *(SDL_Color*)((uint8_t*)surface->pixels + (oy + r * h) * surface->pitch + (ox + c * w) * 4);

							iso = og;
						}
						else
						{
							iso.r = 0;
							iso.g = 0;
							iso.b = 0;
							iso.a = 0;
						}
					}
				}
			}
		}

		return sheet;
	}
	return nullptr;
}
