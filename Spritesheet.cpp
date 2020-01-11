#include "SpriteSheet.h"

#include <SDL.h>
#include <SDL_image.h>

#include <thread>

#include "Vec2.h"

#include "Text.h"

#include <sstream>

SpriteSheet::SpriteSheet()
{
	surface = nullptr;
	texture = nullptr;

	rows = 1;
	columns = 1;

	offset_x = 0;
	offset_y = 0;

	loaded = false;
}

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

SpriteSheet::~SpriteSheet()
{
	if (texture)
		SDL_DestroyTexture(texture);
	if (surface)
		SDL_FreeSurface(surface);
}

std::shared_ptr<SpriteSheet> SpriteSheet::load(const std::string & fname)
{
	auto sheet = std::make_shared<SpriteSheet>();
	std::thread t([sheet, fname]()
		{
			auto meta = Text::get(fname + ".meta");

			sheet->surface = IMG_Load(("data/" + fname).c_str());

			while (!meta->loaded)
				SDL_Delay(0);

			if (meta->size())
			{
				std::istringstream is(*meta);
				is >> sheet->columns >> sheet->rows;
			}

			sheet->loaded = true;
		});
	t.detach();

	return sheet;
}

SDL_Texture * SpriteSheet::getTexture(SDL_Renderer * render)
{
	if (!texture)
		if (loaded)
			if (surface)
				texture = SDL_CreateTextureFromSurface(render, surface);
	return texture;
}

std::shared_ptr<SpriteSheet> SpriteSheet::makeCopy() const
{
	auto sheet = std::make_shared<SpriteSheet>();
	auto shared_this = shared_from_this();
	auto func = [shared_this, sheet]()
	{
		while (!shared_this->loaded)
			SDL_Delay(0);

		if (shared_this->surface)
		{
			auto& surface = shared_this->surface;
			auto& format = surface->format;
			sheet->surface = SDL_CreateRGBSurfaceFrom(surface->pixels, surface->w, surface->h, surface->pitch, format->BitsPerPixel, format->Rmask, format->Gmask, format->Bmask, format->Amask);
			sheet->columns = shared_this->columns;
			sheet->rows = shared_this->rows;
			sheet->offset_x = shared_this->offset_x;
			sheet->offset_y = shared_this->offset_y;
		}

		sheet->loaded = true;
	};

	if (loaded)
	{
		func();
	}
	else
	{
		std::thread t(func);
		t.detach();
	}

	return sheet;
}

std::shared_ptr<SpriteSheet> SpriteSheet::makeIsometricFloorLossy(bool blur) const
{
	auto sheet = std::make_shared<SpriteSheet>();
	auto shared_this = shared_from_this();
	auto func = [shared_this, sheet, blur]()
	{
		while (!shared_this->loaded)
			SDL_Delay(0);

		if (shared_this->surface)
		{
			SDL_Surface * this_surface = shared_this->surface;
			intmax_t this_pitch = this_surface->pitch;
			uint8_t * this_pixels = (uint8_t*)this_surface->pixels;

			intmax_t w = this_surface->w;
			intmax_t h = this_surface->h;
			intmax_t nw = w + h;
			intmax_t nh = nw / 2;

			SDL_Surface * surface = SDL_CreateRGBSurface(0, nw, nh, 32, 0xff, 0xff << 8, 0xff << 16, 0xff << 24);
			intmax_t pitch = surface->pitch;
			uint8_t * pixels = (uint8_t*)surface->pixels;

			for (intmax_t x = 0; x < nw; ++x)
			{
				for (intmax_t y = 0; y < nh; ++y)
				{
					intmax_t ox = (x + 1) / 2 + y + h / 2 - nh;
					intmax_t oy = y - (x + 1) / 2 + h / 2;

					SDL_Color & iso = *(SDL_Color*)(pixels + y * pitch + x * 4);

					if (ox >= 0 && ox < w && oy >= 0 && oy < h)
					{
						SDL_Color & og = *(SDL_Color*)(this_pixels + oy * this_pitch + ox * 4);

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

							SDL_Color & og = *(SDL_Color*)(this_pixels + oy * this_pitch + ox * 4);

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

			sheet->surface = surface;
			sheet->columns = shared_this->columns;
			sheet->rows = shared_this->rows;
			sheet->offset_x = shared_this->offset_x;
			sheet->offset_y = shared_this->offset_y;
		}

		sheet->loaded = true;
	};

	if (loaded)
	{
		func();
	}
	else
	{
		std::thread t(func);
		t.detach();
	}

	return sheet;
}

std::shared_ptr<SpriteSheet> SpriteSheet::makeIsometricFloorLossless(float rotation) const
{
	auto sheet = std::make_shared<SpriteSheet>();
	auto shared_this = shared_from_this();
	auto func = [shared_this, sheet, rotation]()
	{
		while (!shared_this->loaded)
			SDL_Delay(0);

		if (shared_this->surface)
		{
			SDL_Surface * this_surface = shared_this->surface;
			intmax_t this_pitch = this_surface->pitch;
			uint8_t * this_pixels = (uint8_t*)this_surface->pixels;

			intmax_t wf = this_surface->w;
			intmax_t hf = this_surface->h;

			size_t columns = shared_this->columns;
			size_t rows = shared_this->rows;

			intmax_t w = wf / columns;
			intmax_t h = hf / rows;
			intmax_t nw = w + h;
			intmax_t nh = nw / 2;

			intmax_t nwf = nw * columns;
			intmax_t nhf = nh * rows;

			SDL_Surface * surface = SDL_CreateRGBSurface(0, nwf, nhf, 32, 0xff, 0xff << 8, 0xff << 16, 0xff << 24);
			intmax_t pitch = surface->pitch;
			uint8_t * pixels = (uint8_t*)surface->pixels;

			float rads = rotation * M_PI / 180.0f;
			float sin = sinf(rads);
			float cos = cosf(rads);

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

							ox -= w / 2;
							oy -= h / 2;

							intmax_t temp = oy * sin + ox * cos;
							oy = oy * cos - ox * sin;
							ox = temp;

							ox += w / 2;
							oy += h / 2;

							SDL_Color & iso = *(SDL_Color*)(pixels + (y + r * nh) * pitch + (x + c * nw) * 4);

							if (ox >= 0 && ox < w && oy >= 0 && oy < h)
							{
								SDL_Color & og = *(SDL_Color*)(this_pixels + (oy + r * h) * this_pitch + (ox + c * w) * 4);

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

			sheet->surface = surface;
			sheet->columns = columns;
			sheet->rows = rows;
			sheet->offset_x = shared_this->offset_x;
			sheet->offset_y = shared_this->offset_y;
		}

		sheet->loaded = true;
	};

	if (loaded)
	{
		func();
	}
	else
	{
		std::thread t(func);
		t.detach();
	}

	return sheet;
}

std::shared_ptr<SpriteSheet> SpriteSheet::makeScaled(intmax_t scale) const
{
	auto sheet = std::make_shared<SpriteSheet>();
	auto shared_this = shared_from_this();
	auto func = [shared_this, sheet, scale]()
	{
		while (!shared_this->loaded)
			SDL_Delay(0);

		if (shared_this->surface)
		{
			// TODO negative scale, floating point scale, axis scale

			SDL_Surface * this_surface = shared_this->surface;
			intmax_t this_pitch = this_surface->pitch;
			uint8_t * this_pixels = (uint8_t*)this_surface->pixels;

			intmax_t w = this_surface->w;
			intmax_t h = this_surface->h;
			intmax_t nw = w * scale;
			intmax_t nh = h * scale;

			SDL_Surface * surface = SDL_CreateRGBSurface(0, nw, nh, 32, 0xff, 0xff << 8, 0xff << 16, 0xff << 24);
			intmax_t pitch = surface->pitch;
			uint8_t * pixels = (uint8_t*)surface->pixels;

			for (intmax_t x = 0; x < nw; ++x)
			{
				for (intmax_t y = 0; y < nh; ++y)
				{
					intmax_t ox = x / scale;
					intmax_t oy = y / scale;

					SDL_Color & pixel = *(SDL_Color*)(pixels + y * pitch + x * 4);

					SDL_Color & og = *(SDL_Color*)(this_pixels + oy * this_pitch + ox * 4);

					pixel = og;
				}
			}

			sheet->surface = surface;
			sheet->columns = shared_this->columns;
			sheet->rows = shared_this->rows;
			sheet->offset_x = shared_this->offset_x;
			sheet->offset_y = shared_this->offset_y;
		}

		sheet->loaded = true;
	};

	if (loaded)
	{
		func();
	}
	else
	{
		std::thread t(func);
		t.detach();
	}

	return sheet;
}

std::shared_ptr<SpriteSheet> SpriteSheet::makeOutline() const
{
	auto sheet = std::make_shared<SpriteSheet>();
	auto shared_this = shared_from_this();
	auto func = [shared_this, sheet]()
	{
		while (!shared_this->loaded)
			SDL_Delay(0);

		if (shared_this->surface)
		{
			SDL_Surface * this_surface = shared_this->surface;
			intmax_t this_pitch = this_surface->pitch;
			uint8_t * this_pixels = (uint8_t*)this_surface->pixels;

			intmax_t wf = this_surface->w;
			intmax_t hf = this_surface->h;

			size_t columns = shared_this->columns;
			size_t rows = shared_this->rows;

			intmax_t w = wf / columns;
			intmax_t h = hf / rows;
			intmax_t nw = w + 2;
			intmax_t nh = h + 2;

			intmax_t nwf = nw * columns;
			intmax_t nhf = nh * rows;

			SDL_Surface * surface = SDL_CreateRGBSurface(0, nwf, nhf, 32, 0xff, 0xff << 8, 0xff << 16, 0xff << 24);
			intmax_t pitch = surface->pitch;
			uint8_t * pixels = (uint8_t*)surface->pixels;

			for (intmax_t c = 0; c < columns; ++c)
			{
				for (intmax_t r = 0; r < rows; ++r)
				{
					for (intmax_t x = 0; x < nw; ++x)
					{
						for (intmax_t y = 0; y < nh; ++y)
						{
							intmax_t x_min = x - 2;
							if (x_min < 0)
								x_min = 0;
							intmax_t y_min = y - 2;
							if (y_min < 0)
								y_min = 0;

							intmax_t x_max = x + 1;
							if (x_max > w)
								x_max = w;
							intmax_t y_max = y + 1;
							if (y_max > h)
								y_max = h;

							intmax_t opacity = 0;
							for (intmax_t ox = x_min; ox < x_max; ++ox)
							{
								for (intmax_t oy = y_min; oy < y_max; ++oy)
								{
									SDL_Color & og = *(SDL_Color*)(this_pixels + (oy + r * h) * this_pitch + (ox + c * w) * 4);
									opacity += uintmax_t(og.a);
								}
							}

							if (opacity > 255)
								opacity = 255;

							/*if (x != 0 && y != 0 && x != nw - 1 && y != nh - 1)
							{
								SDL_Color & og = *(SDL_Color*)(this_pixels + (y - 1 + r * h) * this_pitch + (x - 1 + c * w) * 4);

								opacity -= og.a;

								if (opacity < 0)
									opacity = 0;
							}*/

							SDL_Color & pixel = *(SDL_Color*)(pixels + (y + r * nh) * pitch + (x + c * nw) * 4);

							pixel = SDL_Color({ 255, 255, 255, (uint8_t)opacity });
						}
					}
				}
			}

			sheet->surface = surface;
			sheet->columns = shared_this->columns;
			sheet->rows = shared_this->rows;
			sheet->offset_x = shared_this->offset_x;
			sheet->offset_y = shared_this->offset_y;
		}

		sheet->loaded = true;
	};

	if (loaded)
	{
		func();
	}
	else
	{
		std::thread t(func);
		t.detach();
	}

	return sheet;
}
