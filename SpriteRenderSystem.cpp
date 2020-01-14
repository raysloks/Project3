#include "SpriteRenderSystem.h"

#include <SDL.h>

#include <iostream>

#include "CustomBehaviour.h"

SpriteRenderSystem::SpriteRenderSystem(SDL_Renderer * render)
{
	//this->sprites = sprites;
	this->render = render;

	camera_position = Vec2();

	target_w = 1920 / 6;
	target_h = target_w;

	offscreen = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, target_w, target_h);

	flicker = false;

	use_offscreen = false;
}

Vec2 SpriteRenderSystem::screenToWorld(const Vec2 & screen_position) const
{
	Vec2 eye = (screen_position - Vec2(screen_w, screen_h) * 0.5f) / scale;
	return Vec2(eye.x * 0.5f + eye.y, eye.y - eye.x * 0.5f) + camera_position;
	// TODO swap around operations to make 'eye' actually contain eye coordinates
}

Vec2 SpriteRenderSystem::worldToScreen(const Vec2 & world_position) const
{
	Vec2 relative_world_position = world_position - camera_position;
	Vec2 eye = relative_world_position * scale + Vec2(screen_w, screen_h) * 0.5f;
	return Vec2(eye.x - eye.y, (eye.x + eye.y) * 0.5f);
	// TODO swap around operations to make 'eye' actually contain eye coordinates
}

void SpriteRenderSystem::tick(float dt)
{
	flicker = !flicker;

	SDL_GetRendererOutputSize(render, &screen_w, &screen_h);

	int scale_new = (screen_w + target_w - 1) / target_w;
	if (scale_new < 1)
		scale_new = 1;
	if (scale_new != scale)
		engine->setCursor(SpriteSheet::get("cursor.png")->makeScaled(scale_new), scale_new, scale_new);
	scale = scale_new;
	
	if (use_offscreen)
	{
		SDL_SetRenderTarget(render, offscreen);
		raw_scale = 1;
	}
	else
	{
		SDL_SetRenderTarget(render, nullptr);
		raw_scale = scale;
	}

	SDL_GetRendererOutputSize(render, &render_w, &render_h);

	std::multimap<float, Sprite*> sorted;

	for (auto& sprite : sprites.components)
		if (sprite.entity)
			if (sprite.sheet->surface)
			{
				auto p = sprite.entity->getPosition();
				sorted.insert(std::make_pair(p.y + p.x + sprite.sort, &sprite));
			}

	Vec2 camera_position_iso(camera_position.x - camera_position.y, (camera_position.y + camera_position.x) * 0.5f);
	camera_position_iso *= raw_scale;
	camera_position_iso -= Vec2(render_w / 2, render_h / 2);
	Vec2 camera_position_iso_raw = camera_position_iso;

	camera_position_iso.x = roundf(camera_position_iso.x);
	camera_position_iso.y = roundf(camera_position_iso.y);

	SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
	SDL_RenderClear(render);

	for (auto& i : sorted)
	{
		auto& sprite = *i.second;

		auto p = sprite.entity->getPosition();

		SDL_Rect src = sprite.sheet->surface->clip_rect;
		src.w /= sprite.sheet->columns;
		src.h /= sprite.sheet->rows;
		src.x += sprite.subsprite_x * src.w;
		src.y += sprite.subsprite_y * src.h;

		Vec2 p_iso(p.x - p.y, (p.y + p.x) * 0.5f - p.z);

		p_iso *= raw_scale;

		p_iso.x = roundf(p_iso.x);
		p_iso.y = roundf(p_iso.y);

		Vec2 flip(sprite.flip & SDL_FLIP_HORIZONTAL ? 1.0f : -1.0f, sprite.flip & SDL_FLIP_VERTICAL ? 1.0f : -1.0f);

		SDL_Rect dst = sprite.sheet->surface->clip_rect;
		dst.w /= sprite.sheet->columns;
		dst.h /= sprite.sheet->rows;

		dst.x = -sprite.sheet->offset_x * flip.x - dst.w / 2;
		dst.y = -sprite.sheet->offset_y * flip.y - dst.h / 2;

		dst.x *= raw_scale;
		dst.y *= raw_scale;
		dst.w *= raw_scale;
		dst.h *= raw_scale;

		dst.x *= sprite.scale.x;
		dst.y *= sprite.scale.y;
		dst.w *= sprite.scale.x;
		dst.h *= sprite.scale.y;

		dst.x += p_iso.x - camera_position_iso.x;
		dst.y += p_iso.y - camera_position_iso.y;

		auto texture = sprite.sheet->getTexture(render);

		SDL_SetTextureColorMod(texture, sprite.color.r, sprite.color.g, sprite.color.b);
		SDL_SetTextureAlphaMod(texture, sprite.color.a);

		SDL_RenderCopyEx(render, texture, &src, &dst, sprite.rotation, nullptr, sprite.flip);
	}

	if (use_offscreen)
	{
		SDL_SetRenderTarget(render, nullptr);

		SDL_Rect rect;
		rect.w = render_w * scale;
		rect.h = render_h * scale;
		rect.x = (camera_position_iso.x - camera_position_iso_raw.x) * scale - (rect.w - screen_w) / 2;
		rect.y = (camera_position_iso.y - camera_position_iso_raw.y) * scale - (rect.h - screen_h) / 2;

		SDL_RenderCopy(render, offscreen, nullptr, &rect);
	}

	effective_w = screen_w / scale;
	effective_h = screen_h / scale;

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

int SpriteRenderSystem::getWidth() const
{
	return effective_w;
}

int SpriteRenderSystem::getHeight() const
{
	return effective_h;
}