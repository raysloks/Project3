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

Vec2 SpriteRenderSystem::screenToWorld(const Vec2 & screen_position)
{
	return (screen_position - Vec2(screen_w, screen_h) * 0.5f) / scale + camera_position;
}

Vec2 SpriteRenderSystem::worldToScreen(const Vec2 & world_position)
{
	return Vec2();
}

void SpriteRenderSystem::tick(float dt)
{
	flicker = !flicker;

	SDL_GetRendererOutputSize(render, &screen_w, &screen_h);

	SDL_SetRenderTarget(render, offscreen);

	SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
	SDL_RenderClear(render);

	SDL_GetRendererOutputSize(render, &w, &h);

	scale = (screen_w + w - 1) / w;

	std::multimap<float, Sprite*> sorted;

	for (auto& sprite : sprites.components)
		if (sprite.entity)
			if (sprite.sheet->surface)
				sorted.insert(std::make_pair(sprite.entity->getPosition().y + sprite.sort, &sprite));

	for (auto& i : sorted)
	{
		auto& sprite = *i.second;

		auto p = sprite.entity->getPosition();

		SDL_Rect src = sprite.sheet->surface->clip_rect;
		src.w /= sprite.sheet->columns;
		src.h /= sprite.sheet->rows;
		src.x += sprite.subsprite_x * src.w;
		src.y += sprite.subsprite_y * src.h;

		SDL_Rect dst = sprite.sheet->surface->clip_rect;
		dst.w /= sprite.sheet->columns;
		dst.h /= sprite.sheet->rows;
		dst.x = (roundf(p.x) - roundf(camera_position.x)) - dst.w / 2 + w / 2;
		dst.y = (roundf(p.y) - roundf(camera_position.y)) - dst.h / 2 + h / 2;

		auto texture = sprite.sheet->getTexture(render);

		SDL_SetTextureColorMod(texture, sprite.color.r, sprite.color.g, sprite.color.b);
		SDL_SetTextureAlphaMod(texture, sprite.color.a);

		SDL_RenderCopyEx(render, texture, &src, &dst, sprite.rotation, nullptr, sprite.flip);
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
