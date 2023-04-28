#include "SpriteSheet.h"

#include <SDL.h>
#include <SDL_image.h>

#include <thread>
#include <mutex>

#include "Vec2.h"
#include "Utf8.h"

#include "Text.h"

#include "ModelRenderSystem.h"

#include <sstream>
#include <iostream>

#include "Coal.h"
#include "Diamond.h"

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

	texture_image = nullptr;
	texture_image_memory = nullptr;
	texture_image_view = nullptr;
	texture_sampler = nullptr;
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

	texture_image = nullptr;
	texture_image_memory = nullptr;
	texture_image_view = nullptr;
	texture_sampler = nullptr;
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

			sheet->surface = IMG_Load((BaseResource::data_location + fname).c_str());

			meta->loaded.wait(false);

			if (meta->size())
			{
				std::istringstream is(*meta);
				auto coal = Coal::parse(is);
				if (coal)
				{
					for (auto& element : coal->members["frames"].elements)
					{
						size_t index = element["index"];
						Frame frame;
						Diamond::init(frame.min, element["min"]);
						Diamond::init(frame.max, element["max"]);
						Diamond::init(frame.center, element["center"]);
						Diamond::init(frame.advance, element["advance"]);
						sheet->frames[(uint32_t)index] = frame;
					}
				}
			}

			sheet->loaded = true;
			sheet->loaded.notify_all();
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
		shared_this->loaded.wait(false);

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

void SpriteSheet::createTexture(ModelRenderSystem * mrs)
{
	if (texture_image == nullptr)
	{
		createTextureImage(mrs);
		createTextureImageView(mrs);
		createTextureSampler(mrs);
	}
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

	texture_image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
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

std::shared_ptr<SpriteSheet> SpriteSheet::merge(const std::shared_ptr<SpriteSheet>& fg) const
{
	auto sheet = std::make_shared<SpriteSheet>();
	auto shared_this = shared_from_this();
	auto func = [shared_this, sheet, fg]()
	{
		shared_this->loaded.wait(false);
		fg->loaded.wait(false);

		if (shared_this->surface && fg->surface)
		{
			auto& bg_surf = shared_this->surface;
			auto& bg_form = bg_surf->format;
			auto& fg_surf = fg->surface;
			auto& fg_form = fg_surf->format;

			intmax_t w = bg_surf->w;
			intmax_t h = bg_surf->h;

			sheet->surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, bg_form->BitsPerPixel, bg_form->format);
			auto& surf = sheet->surface;
			auto& form = surf->format;

			intmax_t dx = intmax_t(fg->offset_x) - shared_this->offset_x;
			intmax_t dy = intmax_t(fg->offset_y) - shared_this->offset_y;

			intmax_t fg_pixel_offset = dy * fg_surf->pitch + dx * 4;

			for (intmax_t y = 0; y < h; ++y)
			{
				for (intmax_t x = 0; x < w; ++x)
				{
					SDL_Color& bg_color = *(SDL_Color*)((char*)bg_surf->pixels + y * bg_surf->pitch + x * 4);
					SDL_Color& fg_color = *(SDL_Color*)((char*)fg_surf->pixels + y * fg_surf->pitch + x * 4 + fg_pixel_offset);
					SDL_Color& color = *(SDL_Color*)((char*)surf->pixels + y * surf->pitch + x * 4);

					if (x + dx >= 0 && x + dx < fg_surf->w && y + dy >= 0 && y + dy < fg_surf->h)
					{
						color.r = std::max(intmax_t(0), intmax_t(fg_color.r) + (intmax_t(bg_color.r) * (intmax_t(255) - intmax_t(fg_color.a)) + intmax_t(127)) / intmax_t(255));
						color.g = std::max(intmax_t(0), intmax_t(fg_color.g) + (intmax_t(bg_color.g) * (intmax_t(255) - intmax_t(fg_color.a)) + intmax_t(127)) / intmax_t(255));
						color.b = std::max(intmax_t(0), intmax_t(fg_color.b) + (intmax_t(bg_color.b) * (intmax_t(255) - intmax_t(fg_color.a)) + intmax_t(127)) / intmax_t(255));
						color.a = intmax_t(bg_color.a) + intmax_t(fg_color.a) - (intmax_t(bg_color.a) * intmax_t(fg_color.a) + intmax_t(127)) / intmax_t(255);
					}
					else
					{
						color = bg_color;
					}
				}
			}

			sheet->columns = shared_this->columns;
			sheet->rows = shared_this->rows;
			sheet->offset_x = shared_this->offset_x;
			sheet->offset_y = shared_this->offset_y;
		}

		sheet->loaded = true;
		sheet->loaded.notify_all();
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

std::shared_ptr<SpriteSheet> SpriteSheet::makeCopy() const
{
	auto sheet = std::make_shared<SpriteSheet>();
	auto shared_this = shared_from_this();
	auto func = [shared_this, sheet]()
	{
		shared_this->loaded.wait(false);

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
		sheet->loaded.notify_all();
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
		shared_this->loaded.wait(false);
		map->loaded.wait(false);

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
		sheet->loaded.notify_all();
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
		shared_this->loaded.wait(false);

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
		sheet->loaded.notify_all();
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
		shared_this->loaded.wait(false);

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
		sheet->loaded.notify_all();
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
		shared_this->loaded.wait(false);

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
		sheet->loaded.notify_all();
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
		shared_this->loaded.wait(false);

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
		sheet->loaded.notify_all();
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

std::shared_ptr<SpriteSheet> SpriteSheet::makeOutline(const SDL_Color & outline_color, const SDL_Color & fill_color, intmax_t cell_margin, intmax_t outline_width) const
{
	auto sheet = std::make_shared<SpriteSheet>();
	auto shared_this = shared_from_this();
	auto func = [shared_this, sheet, outline_color, fill_color, cell_margin, outline_width]()
	{
		shared_this->loaded.wait(false);

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
			intmax_t nw = w + cell_margin * 2;
			intmax_t nh = h + cell_margin * 2;

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
							intmax_t x_min = x - cell_margin - outline_width;
							if (x_min < 0)
								x_min = 0;
							intmax_t y_min = y - cell_margin - outline_width;
							if (y_min < 0)
								y_min = 0;

							intmax_t x_max = x - cell_margin + 1 + outline_width;
							if (x_max > w)
								x_max = w;
							intmax_t y_max = y - cell_margin + 1 + outline_width;
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
								color.a = std::min(opacity, 255ll) * outline_color.a / 255;
							}

							// hoping the compiler can optimize this into first/last loops
							if (x >= cell_margin && x < nw - cell_margin && y >= cell_margin && y < nh - cell_margin)
							{
								SDL_Color & og = *(SDL_Color*)(this_pixels + (y - cell_margin + r * h) * this_pitch + (x - cell_margin + c * w) * 4);
								if (og.a > 0)
								{
									intmax_t one_minus_og_a = 255ll - og.a;
									// hopefully this also gets optimized out
									if (fill_color.a == 0 && fill_color.r > 0)
									{
										color.r = (color.r * one_minus_og_a + intmax_t(og.r) * og.a) / 255ll;
										color.g = (color.g * one_minus_og_a + intmax_t(og.g) * og.a) / 255ll;
										color.b = (color.b * one_minus_og_a + intmax_t(og.b) * og.a) / 255ll;
										color.a = 255ll - (255ll - color.a) * one_minus_og_a / 255ll;
									}
									else
									{
										color.r = (color.r * one_minus_og_a + intmax_t(fill_color.r) * og.a) / 255ll;
										color.g = (color.g * one_minus_og_a + intmax_t(fill_color.g) * og.a) / 255ll;
										color.b = (color.b * one_minus_og_a + intmax_t(fill_color.b) * og.a) / 255ll;
										color.a = 255ll - (255ll - color.a) * one_minus_og_a / 255ll; // shieeet??
									}
								}
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
		sheet->loaded.notify_all();
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

std::shared_ptr<Model> SpriteSheet::makeTextModel(const std::string& text, const TextSettings& settings) const
{
	auto model = std::make_shared<Model>();
	auto shared_this = shared_from_this();
	auto func = [shared_this, model, text, settings]()
	{
		shared_this->loaded.wait(false);

		auto code_points = getCodePoints(text);

		Vec2 offset;
		model->vertices.resize(code_points.size() * 4);
		model->triangles.resize(code_points.size() * 2);
		size_t i = 0;
		bool justified = false;
		for (auto c = code_points.begin(); c != code_points.end(); ++c)
		{
			if (!justified)
			{
				Vec2 line_advance = shared_this->getLineAdvance(c, code_points.end(), settings);
				offset -= line_advance * settings.justification;
				justified = true;
			}

			auto it = shared_this->frames.find(*c);
			if (it == shared_this->frames.end())
			{
				it = shared_this->frames.find(0xfffd); // replacement character
				if (it == shared_this->frames.end())
					it = shared_this->frames.begin();
			}
			auto& frame = it->second;

			if (frame.min != frame.max)
			{
				model->vertices[i * 4 + 0].uv = frame.min;
				model->vertices[i * 4 + 1].uv = Vec2(frame.max.x, frame.min.y);
				model->vertices[i * 4 + 2].uv = frame.max;
				model->vertices[i * 4 + 3].uv = Vec2(frame.min.x, frame.max.y);
				model->vertices[i * 4 + 0].position = Vec2(frame.min.x, frame.max.y);
				model->vertices[i * 4 + 1].position = frame.max;
				model->vertices[i * 4 + 2].position = Vec2(frame.max.x, frame.min.y);
				model->vertices[i * 4 + 3].position = frame.min;
				for (size_t j = i * 4; j < i * 4 + 4; ++j)
				{
					auto& vertex = model->vertices[j];
					vertex.position -= frame.center;
					vertex.position *= Vec2(shared_this->surface->w, shared_this->surface->h);
					vertex.position += offset;
					vertex.normal = Vec3(0.0f, 0.0f, 1.0f);
					vertex.bones[0] = 0;
					vertex.bones[1] = 0;
					vertex.bones[2] = 0;
					vertex.bones[3] = 0;
					vertex.weights[0] = 1.0f;
					vertex.weights[1] = 0.0f;
					vertex.weights[2] = 0.0f;
					vertex.weights[3] = 0.0f;
				}

				model->triangles[i * 2 + 0].indices[0] = i * 4 + 1;
				model->triangles[i * 2 + 0].indices[1] = i * 4 + 0;
				model->triangles[i * 2 + 0].indices[2] = i * 4 + 2;
				model->triangles[i * 2 + 1].indices[2] = i * 4 + 3;
				model->triangles[i * 2 + 1].indices[0] = i * 4 + 2;
				model->triangles[i * 2 + 1].indices[1] = i * 4 + 0;

				++i;
			}

			offset += frame.advance;

			if (*c == 0x0a) // LF
			{
				offset.x = 0.0f;
				offset.y -= settings.line_height;
				justified = false;
			}
		}
		model->vertices.resize(i * 4);
		model->triangles.resize(i * 2);

		model->loaded = true;
		model->loaded.notify_all();
	};

	if (loaded)
		func();
	else
		thread_pool.push(func);

	return model;
}

std::shared_ptr<Model> SpriteSheet::makeFrameModel(uint32_t frame_index, bool normalize) const
{
	auto model = std::make_shared<Model>();
	auto shared_this = shared_from_this();
	auto func = [shared_this, model, frame_index, normalize]()
	{
		shared_this->loaded.wait(false);

		model->vertices.resize(4);
		model->triangles.resize(2);
		auto it = shared_this->frames.find(frame_index);
		if (it != shared_this->frames.end())
		{
			auto& frame = it->second;

			model->vertices[0].uv = frame.min;
			model->vertices[1].uv = Vec2(frame.max.x, frame.min.y);
			model->vertices[2].uv = frame.max;
			model->vertices[3].uv = Vec2(frame.min.x, frame.max.y);

			if (normalize)
			{
				model->vertices[0].position = Vec2(0.0f, 1.0f);
				model->vertices[1].position = Vec2(1.0f, 1.0f);
				model->vertices[2].position = Vec2(1.0f, 0.0f);
				model->vertices[3].position = Vec2(0.0f, 0.0f);
			}
			else
			{
				model->vertices[0].position = Vec2(frame.min.x, frame.max.y);
				model->vertices[1].position = frame.max;
				model->vertices[2].position = Vec2(frame.max.x, frame.min.y);
				model->vertices[3].position = frame.min;
			}

			for (size_t j = 0; j < 4; ++j)
			{
				auto& vertex = model->vertices[j];
				if (normalize == false)
				{
					vertex.position -= frame.center;
					vertex.position *= Vec2(shared_this->surface->w, shared_this->surface->h);
				}
				vertex.normal = Vec3(0.0f, 0.0f, 1.0f);
				vertex.bones[0] = 0;
				vertex.bones[1] = 0;
				vertex.bones[2] = 0;
				vertex.bones[3] = 0;
				vertex.weights[0] = 1.0f;
				vertex.weights[1] = 0.0f;
				vertex.weights[2] = 0.0f;
				vertex.weights[3] = 0.0f;
			}

			model->triangles[0].indices[0] = 1;
			model->triangles[0].indices[1] = 0;
			model->triangles[0].indices[2] = 2;
			model->triangles[1].indices[2] = 3;
			model->triangles[1].indices[0] = 2;
			model->triangles[1].indices[1] = 0;
		}

		model->loaded = true;
		model->loaded.notify_all();
	};

	if (loaded)
		func();
	else
		thread_pool.push(func);

	return model;
}

Vec2 SpriteSheet::getAdvance(const std::string& text, const TextSettings& settings, size_t index) const
{
	loaded.wait(false);

	Vec2 advance;
	bool justified = false;
	for (auto c = text.begin(); c != text.end();)
	{
		if (!justified)
		{
			Vec2 line_advance = getLineAdvance(c, text.end(), settings);
			advance -= line_advance * settings.justification;
			justified = true;
		}

		if (c - text.begin() == index)
			return advance;

		auto code_point = getCodePoint(c, text.end());
		auto it = frames.find(code_point);
		if (it == frames.end())
		{
			it = frames.find(0xfffd); // replacement character
			if (it == frames.end())
				it = frames.begin();
		}
		auto& frame = it->second;
		advance += frame.advance;
		if (code_point == 0x0a) // LF
		{
			advance.x = 0.0f;
			advance.y += settings.line_height;
			justified = false;
		}
	}
	return advance;
}

Vec2 SpriteSheet::getJustifiedOffset(const std::string& text, const TextSettings& settings, size_t index) const
{
	auto code_points = getCodePoints(text);

	loaded.wait(false);

	Vec2 advance, bounds;
	for (auto c = code_points.begin(); c != code_points.end(); ++c)
	{
		auto it = frames.find(*c);
		if (it == frames.end())
		{
			it = frames.find(0xfffd); // replacement character
			if (it == frames.end())
				it = frames.begin();
		}
		auto& frame = it->second;
		advance += frame.advance;
		if (*c == 0x0a) // LF
		{
			advance.x = 0.0f;
			advance.y += settings.line_height;
		}
		bounds.x = std::max(bounds.x, advance.x);
		bounds.y = std::max(bounds.y, advance.y);
	}
	return bounds * (Vec2(1.0f) - settings.justification * 2.0f);
}

std::vector<Vec2> SpriteSheet::getLineAdvances(const std::string& text) const
{
	auto code_points = getCodePoints(text);

	loaded.wait(false);

	std::vector<Vec2> advances;
	advances.push_back(Vec2());
	for (auto& c : code_points)
	{
		auto it = frames.find(c);
		if (it == frames.end())
		{
			it = frames.find(0xfffd); // replacement character
			if (it == frames.end())
				it = frames.begin();
		}
		if (c == 0x0a) // LF
			advances.push_back(Vec2());
		auto& frame = it->second;
		advances.back() += frame.advance;
	}
	return advances;
}

int64_t SpriteSheet::getIndexFromPosition(const std::string& text, const TextSettings& settings, const Vec2& position) const
{
	loaded.wait(false);

	Vec2 advance;
	bool justified = false;
	for (auto c = text.begin(); c != text.end();)
	{
		if (!justified)
		{
			Vec2 line_advance = getLineAdvance(c, text.end(), settings);
			advance -= line_advance * settings.justification;
			justified = true;
		}

		auto prev = c;
		auto code_point = getCodePoint(c, text.end());
		auto it = frames.find(code_point);
		if (it == frames.end())
		{
			it = frames.find(0xfffd); // replacement character
			if (it == frames.end())
				it = frames.begin();
		}
		auto& frame = it->second;

		if (advance + frame.advance * 0.5f > position)
			return prev - text.begin();

		advance += frame.advance;

		if (code_point == 0x0a) // LF
		{
			if (advance.y > position.y)
				return prev - text.begin();
			advance.x = 0.0f;
			advance.y += settings.line_height;
			justified = false;
		}
	}
	return text.size();
}

Vec2 SpriteSheet::getLineAdvance(const std::u32string::const_iterator& begin, const std::u32string::const_iterator& end, const TextSettings& settings) const
{
	loaded.wait(false);

	Vec2 advance;
	for (std::u32string::const_iterator c = begin; c != end; ++c)
	{
		auto it = frames.find(*c);
		if (it == frames.end())
		{
			it = frames.find(0xfffd); // replacement character
			if (it == frames.end())
				it = frames.begin();
		}
		if (*c == 0x0a) // LF
			return advance;
		auto& frame = it->second;
		advance += frame.advance;
	}
	return advance;
}

Vec2 SpriteSheet::getLineAdvance(const std::string::const_iterator& begin, const std::string::const_iterator& end, const TextSettings& settings) const
{
	loaded.wait(false);

	Vec2 advance;
	for (std::string::const_iterator c = begin; c != end;)
	{
		auto code_point = getCodePoint(c, end);
		auto it = frames.find(code_point);
		if (it == frames.end())
		{
			it = frames.find(0xfffd); // replacement character
			if (it == frames.end())
				it = frames.begin();
		}
		if (code_point == 0x0a) // LF
			return advance;
		auto& frame = it->second;
		advance += frame.advance;
	}
	return advance;
}

