#include "SpriteSheet.h"

#include <SDL.h>
#include <SDL_image.h>

#include <thread>
#include <mutex>

#include "Vec2.h"

#include "Text.h"

#include "ModelRenderSystem.h"

#include <sstream>
#include <iostream>

std::vector<SDL_Texture*> unused_textures;
std::mutex unused_textures_mutex;

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
	{
		unused_textures_mutex.lock();
		unused_textures.push_back(texture);
		unused_textures_mutex.unlock();
	}
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
				std::this_thread::yield();

			if (meta->size())
			{
				std::istringstream is(*meta);
				is >> sheet->columns >> sheet->rows >> sheet->offset_x >> sheet->offset_y;
			}

			sheet->loaded = true;
		});
	t.detach();

	return sheet;
}

void SpriteSheet::destroyUnusedTextures()
{
	unused_textures_mutex.lock();
	for (auto texture : unused_textures)
		SDL_DestroyTexture(texture);
	unused_textures.clear();
	unused_textures_mutex.unlock();
}

void SpriteSheet::save(const std::string & fname) const
{
	auto shared_this = shared_from_this();
	auto func = [shared_this, fname]()
	{
		while (!shared_this->loaded)
			SDL_Delay(0);

		if (shared_this->surface)
		{
			IMG_SavePNG(shared_this->surface, fname.c_str());
		}
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
}

SDL_Texture * SpriteSheet::getTexture(SDL_Renderer * render)
{
	if (!texture)
		if (loaded)
			if (surface)
				texture = SDL_CreateTextureFromSurface(render, surface);
	return texture;
}

void SpriteSheet::createTextureImage(ModelRenderSystem * mrs)
{
	uint32_t width = surface->w;
	uint32_t height = surface->h;
	VkDeviceSize buffer_size = (VkDeviceSize)width * height * 4;

	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_memory;
	mrs->createBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);

	void * data;
	vkMapMemory(mrs->getDevice(), staging_buffer_memory, 0, buffer_size, 0, &data);
	memcpy(data, surface->pixels, (size_t)buffer_size);
	vkUnmapMemory(mrs->getDevice(), staging_buffer_memory);

	mrs->createImage(width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, texture_image, texture_image_memory);

	mrs->transitionImageLayout(texture_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	mrs->copyBufferToImage(staging_buffer, texture_image, width, height);

	mrs->transitionImageLayout(texture_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(mrs->getDevice(), staging_buffer, nullptr);
	vkFreeMemory(mrs->getDevice(), staging_buffer_memory, nullptr);
}

void SpriteSheet::createTextureImageView(ModelRenderSystem * mrs)
{
	mrs->createImageView(texture_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, texture_image_view);
}

void SpriteSheet::createTextureSampler(ModelRenderSystem * mrs)
{
	VkPhysicalDeviceProperties physical_device_properties;
	vkGetPhysicalDeviceProperties(mrs->getPhysicalDevice(), &physical_device_properties);

	VkSamplerCreateInfo sampler_create_info = {
		VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		nullptr,
		0,
		VK_FILTER_LINEAR,
		VK_FILTER_LINEAR,
		VK_SAMPLER_MIPMAP_MODE_LINEAR,
		VK_SAMPLER_ADDRESS_MODE_REPEAT,
		VK_SAMPLER_ADDRESS_MODE_REPEAT,
		VK_SAMPLER_ADDRESS_MODE_REPEAT,
		0.0f,
		VK_TRUE,
		physical_device_properties.limits.maxSamplerAnisotropy,
		VK_FALSE,
		VK_COMPARE_OP_ALWAYS,
		0.0f,
		0.0f,
		VK_BORDER_COLOR_INT_OPAQUE_BLACK,
		VK_FALSE
	};

	if (vkCreateSampler(mrs->getDevice(), &sampler_create_info, nullptr, &texture_sampler))
		throw std::runtime_error("failed to create texture sampler.");
}

std::shared_ptr<SpriteSheet> SpriteSheet::createIsometricFloorLosslessMap(size_t w, size_t h)
{
	auto sheet = std::make_shared<SpriteSheet>();

	intmax_t nw = w + h;
	intmax_t nh = nw / 2;

	SDL_Surface * surface = SDL_CreateRGBSurface(0, nw, nh, 32, 0xff, 0xff << 8, 0xff << 16, 0xff << 24);
	intmax_t pitch = surface->pitch;
	uint8_t * pixels = (uint8_t*)surface->pixels;

	sheet->surface = surface;

	/*float rads = rotation * M_PI / 180.0f;
	float sin = sinf(rads);
	float cos = cosf(rads);*/

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

			/*ox -= w / 2;
			oy -= h / 2;

			intmax_t temp = oy * sin + ox * cos;
			oy = oy * cos - ox * sin;
			ox = temp;

			ox += w / 2;
			oy += h / 2;*/

			SDL_Color & iso = *(SDL_Color*)(pixels + y * pitch + x * 4);

			if (ox >= 0 && ox < w && oy >= 0 && oy < h)
			{
				iso.r = ox;
				iso.g = oy;
				iso.b = 0;
				iso.a = 255;
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

	sheet->loaded = true;

	return sheet;
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
			sheet->surface = SDL_CreateRGBSurfaceWithFormat(0, surface->w, surface->h, format->BitsPerPixel, format->format);
			memcpy(sheet->surface->pixels, surface->pixels, format->BytesPerPixel * surface->pitch * surface->h);
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

std::shared_ptr<SpriteSheet> SpriteSheet::makeMapped(const std::shared_ptr<SpriteSheet>& map) const
{
	auto sheet = std::make_shared<SpriteSheet>();
	auto shared_this = shared_from_this();
	auto func = [shared_this, sheet, map]()
	{
		while (!(shared_this->loaded && map->loaded))
			SDL_Delay(0);

		if (shared_this->surface && map->surface)
		{
			auto& this_surface = shared_this->surface;
			auto& this_format = this_surface->format;
			intmax_t this_pitch = this_surface->pitch;
			uint8_t * this_pixels = (uint8_t*)this_surface->pixels;

			auto& map_surface = map->surface;
			auto& map_format = map_surface->format;
			intmax_t map_pitch = map_surface->pitch;
			uint8_t * map_pixels = (uint8_t*)map_surface->pixels;

			intmax_t map_w = map_surface->w;
			intmax_t map_h = map_surface->h;

			SDL_Surface * surface = SDL_CreateRGBSurface(0, map_w, map_h, this_format->BitsPerPixel, this_format->Rmask, this_format->Gmask, this_format->Bmask, this_format->Amask);
			intmax_t pitch = surface->pitch;
			uint8_t * pixels = (uint8_t*)surface->pixels;

			for (intmax_t x = 0; x < map_w; ++x)
			{
				for (intmax_t y = 0; y < map_h; ++y)
				{
					SDL_Color & co = *(SDL_Color*)(map_pixels + y * map_pitch + x * 4);

					SDL_Color & pixel = *(SDL_Color*)(pixels + y * pitch + x * 4);

					if (co.a > 0)
					{
						SDL_Color & og = *(SDL_Color*)(this_pixels + intmax_t(co.g) * this_pitch + intmax_t(co.r) * 4);

						pixel = og;
						//pixel = { uint8_t(co.r * 16), uint8_t(co.g * 16), 0, 255 };
					}
					else
					{
						pixel = { 0, 0, 0, 0 };
					}
				}
			}

			sheet->surface = surface;
			sheet->columns = map->columns;
			sheet->rows = map->rows;
			sheet->offset_x = map->offset_x;
			sheet->offset_y = map->offset_y;
		}

		sheet->loaded = true;
	};

	if (loaded && map->loaded)
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

std::shared_ptr<SpriteSheet> SpriteSheet::makeIsometricWall() const
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
			intmax_t nw = w;
			intmax_t nh = h + (w + 0) / 2;

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
							intmax_t ox = x;
							intmax_t oy = y - (x + 0) / 2;

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

#include <iostream>

std::shared_ptr<SpriteSheet> SpriteSheet::makeOutline(const SDL_Color & outline_color, const SDL_Color & fill_color) const
{
	auto sheet = std::make_shared<SpriteSheet>();
	auto shared_this = shared_from_this();
	auto func = [shared_this, sheet, outline_color, fill_color]()
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
									opacity += og.a;
								}
							}

							SDL_Color & pixel = *(SDL_Color*)(pixels + (y + r * nh) * pitch + (x + c * nw) * 4);

							SDL_Color color = { 0, 0, 0, 0 };

							if (opacity > 0)
							{
								color = outline_color;
							}

							// hoping the compiler can optimize this into first/last loops
							if (x != 0 && x != nw - 1 && y != 0 && y != nh - 1)
							{
								SDL_Color & og = *(SDL_Color*)(this_pixels + (y - 1 + r * h) * this_pitch + (x - 1 + c * w) * 4);
								if (og.a > 0)
									color = fill_color;
							}

							pixel = color;

							/*if (x != 0 && y != 0 && x != nw - 1 && y != nh - 1)
							{
								SDL_Color & og = *(SDL_Color*)(this_pixels + (y - 1 + r * h) * this_pitch + (x - 1 + c * w) * 4);

								opacity -= og.a;

								if (opacity < 0)
									opacity = 0;
							}*/
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
