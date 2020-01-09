#include "SpriteRenderSystem.h"

#include <SDL.h>

#include <iostream>

#include "CustomBehaviour.h"

SpriteRenderSystem::SpriteRenderSystem(SDL_Renderer * render)
{
	//this->sprites = sprites;
	this->render = render;

	camera_position = Vec2();

	offscreen = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 512, 256 + 32);

	flicker = false;
}

Vec2 SpriteRenderSystem::screenToWorld(const Vec2 & p)
{
	Vec2 eye = (p - Vec2(screen_w, screen_h) * 0.5f) / scale;
	return Vec2(eye.x * 0.5f + eye.y, eye.y - eye.x * 0.5f) + camera_position;
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

	int scale_new = (screen_w + w - 1) / w;
	if (scale_new < 1)
		scale_new = 1;
	if (scale_new != scale)
		CustomBehaviour::engine->setCursor(SpriteSheet::get("cursor.png")->makeScaled(scale_new)); // TODO SUPER HACKY fix asap
	scale = scale_new;

	std::multimap<float, Sprite*> sorted;

	for (auto& sprite : sprites.components)
		if (sprite.entity)
			if (sprite.sheet->surface)
			{
				auto p = sprite.entity->getPosition();
				sorted.insert(std::make_pair(p.y + p.x + sprite.sort, &sprite));
			}

	Vec2 camera_position_iso(camera_position.x - camera_position.y, (camera_position.y + camera_position.x) * 0.5f);

	for (auto& i : sorted)
	{
		auto& sprite = *i.second;

		auto p = sprite.entity->getPosition();

		SDL_Rect src = sprite.sheet->surface->clip_rect;
		src.w /= sprite.sheet->columns;
		src.h /= sprite.sheet->rows;
		src.x += sprite.subsprite_x * src.w;
		src.y += sprite.subsprite_y * src.h;

		Vec2 p_iso(p.x - p.y, (p.y + p.x) * 0.5f);

		Vec2 flip(sprite.flip & SDL_FLIP_HORIZONTAL ? 1.0f : -1.0f, sprite.flip & SDL_FLIP_VERTICAL ? 1.0f : -1.0f);

		SDL_Rect dst = sprite.sheet->surface->clip_rect;
		dst.w /= sprite.sheet->columns;
		dst.h /= sprite.sheet->rows;
		dst.x = (roundf(p_iso.x) - roundf(camera_position_iso.x)) - sprite.sheet->offset_x * flip.x - dst.w / 2 + w / 2;
		dst.y = (roundf(p_iso.y) - roundf(camera_position_iso.y)) - sprite.sheet->offset_y * flip.y - dst.h / 2 + h / 2;

		auto texture = sprite.sheet->getTexture(render);

		SDL_SetTextureColorMod(texture, sprite.color.r, sprite.color.g, sprite.color.b);
		SDL_SetTextureAlphaMod(texture, sprite.color.a);

		SDL_RenderCopyEx(render, texture, &src, &dst, sprite.rotation, nullptr, sprite.flip);
	}

	SDL_SetRenderTarget(render, nullptr);

	SDL_Rect rect;
	rect.w = w * scale;
	rect.h = h * scale;
	rect.x = (roundf(camera_position_iso.x) - camera_position_iso.x) * scale - (rect.w - screen_w) / 2;
	rect.y = (roundf(camera_position_iso.y) - camera_position_iso.y) * scale - (rect.h - screen_h) / 2;

	SDL_RenderCopy(render, offscreen, nullptr, &rect);

	for (auto& sprite : ui.components)
	{
		if (sprite.entity)
		{
			if (sprite.sheet->surface)
			{
				auto p = sprite.entity->getPosition();

				SDL_Rect src = sprite.sheet->surface->clip_rect;
				src.w /= sprite.sheet->columns;
				src.h /= sprite.sheet->rows;
				src.x += sprite.subsprite_x * src.w;
				src.y += sprite.subsprite_y * src.h;

				Vec2 flip(sprite.flip & SDL_FLIP_HORIZONTAL ? 1.0f : -1.0f, sprite.flip & SDL_FLIP_VERTICAL ? 1.0f : -1.0f);

				SDL_Rect dst = sprite.sheet->surface->clip_rect;
				dst.w /= sprite.sheet->columns;
				dst.h /= sprite.sheet->rows;
				dst.x = roundf(p.x) - sprite.sheet->offset_x * flip.x - dst.w / 2;
				dst.y = roundf(p.y) - sprite.sheet->offset_y * flip.y - dst.h / 2;
				dst.w *= scale;
				dst.h *= scale;
				dst.x *= scale;
				dst.y *= scale;

				auto texture = sprite.sheet->getTexture(render);

				SDL_SetTextureColorMod(texture, sprite.color.r, sprite.color.g, sprite.color.b);
				SDL_SetTextureAlphaMod(texture, sprite.color.a);

				SDL_RenderCopyEx(render, texture, &src, &dst, sprite.rotation, nullptr, sprite.flip);
			}
		}
	}
}
