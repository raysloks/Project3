#include "SpriteRenderSystem.h"

#include <SDL.h>

#include "Spritesheet.h"

#include <iostream>

SpriteRenderSystem::SpriteRenderSystem(SDL_Renderer * render)
{
	//this->sprites = sprites;
	this->render = render;

	camera_position = Vec2();

	flicker = false;
}

void SpriteRenderSystem::tick(float dt)
{
	flicker = !flicker;

	int w, h;
	SDL_GetRendererOutputSize(render, &w, &h);

	std::multimap<float, Sprite*> sorted;

	for (auto& sprite : sprites.components)
		if (sprite.entity)
			sorted.insert(std::make_pair(sprite.entity->p.y, &sprite));

	for (auto i : sorted)
	{
		auto& sprite = *i.second;
		/*if (flicker)
			SDL_SetTextureColorMod(sprite.texture, 255, 0, 0);
		else
			SDL_SetTextureColorMod(sprite.texture, 0, 255, 255);*/
		SDL_Rect rect = sprite.sheet->surface->clip_rect;
		rect.x = (sprite.entity->p.x - camera_position.x) - rect.w / 2 + w / 2;
		rect.y = (sprite.entity->p.y - camera_position.y) - rect.h / 2 + h / 2;
		//rect.w = sprite.rect.w;
		//rect.h = sprite.rect.h;
		SDL_RenderCopy(render, sprite.sheet->getTexture(render), nullptr, &rect);
	}
}
