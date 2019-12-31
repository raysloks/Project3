#include "SpriteRenderSystem.h"

#include <SDL.h>

#include <iostream>

SpriteRenderSystem::SpriteRenderSystem(SDL_Renderer * render)
{
	//this->sprites = sprites;
	this->render = render;

	camera_position = Vec2();

	offscreen = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 512, 512);

	flicker = false;
}

void SpriteRenderSystem::tick(float dt)
{
	flicker = !flicker;

	int screen_w, screen_h;
	SDL_GetRendererOutputSize(render, &screen_w, &screen_h);

	SDL_SetRenderTarget(render, offscreen);

	SDL_SetRenderDrawColor(render, 120, 180, 60, 255);
	SDL_RenderClear(render);

	int w, h;
	SDL_GetRendererOutputSize(render, &w, &h);

	int scale = (screen_w + w - 1) / w;

	std::multimap<float, Sprite*> sorted;

	for (auto& sprite : sprites.components)
		if (sprite.entity)
			if (sprite.sheet->surface)
				sorted.insert(std::make_pair(sprite.entity->p.y + sprite.sort, &sprite));

	for (auto i : sorted)
	{
		auto& sprite = *i.second;

		SDL_Rect src = sprite.sheet->surface->clip_rect;
		src.w /= sprite.sheet->columns;
		src.h /= sprite.sheet->rows;
		src.x += sprite.subsprite_x * src.w;
		src.y += sprite.subsprite_y * src.h;

		SDL_Rect dst = sprite.sheet->surface->clip_rect;
		dst.w /= sprite.sheet->columns;
		dst.h /= sprite.sheet->rows;
		dst.x = (roundf(sprite.entity->p.x) - roundf(camera_position.x)) - dst.w / 2 + w / 2;
		dst.y = (roundf(sprite.entity->p.y) - roundf(camera_position.y)) - dst.h / 2 + h / 2;

		SDL_RenderCopy(render, sprite.sheet->getTexture(render), &src, &dst);
	}

	SDL_SetRenderTarget(render, nullptr);

	SDL_Rect rect;
	rect.w = w * scale;
	rect.h = h * scale;
	rect.x = (roundf(camera_position.x) - camera_position.x) * scale - (rect.w - screen_w) / 2;
	rect.y = (roundf(camera_position.y) - camera_position.y) * scale - (rect.h - screen_h) / 2;

	SDL_RenderCopy(render, offscreen, nullptr, &rect);

	for (auto& sprite : ui.components)
	{
		if (sprite.entity)
		{
			if (sprite.sheet->surface)
			{
				SDL_Rect src = sprite.sheet->surface->clip_rect;
				src.w /= sprite.sheet->columns;
				src.h /= sprite.sheet->rows;
				src.x += sprite.subsprite_x * src.w;
				src.y += sprite.subsprite_y * src.h;

				SDL_Rect dst = sprite.sheet->surface->clip_rect;
				dst.w /= sprite.sheet->columns;
				dst.h /= sprite.sheet->rows;
				dst.x = sprite.entity->p.x - dst.w / 2;
				dst.y = sprite.entity->p.y - dst.h / 2;
				dst.w *= scale;
				dst.h *= scale;
				dst.x *= scale;
				dst.y *= scale;

				SDL_RenderCopy(render, sprite.sheet->getTexture(render), &src, &dst);
			}
		}
	}
}
