#include "SpriteRenderSystem.h"

#include <SDL.h>

SpriteRenderSystem::SpriteRenderSystem(SDL_Renderer * render)
{
	//this->sprites = sprites;
	this->render = render;
}

void SpriteRenderSystem::tick(float dt)
{
	SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
	for (auto sprite : sprites.components)
	{
		SDL_Rect rect;
		rect.x = sprite.entity->p.x - camera_position.x;
		rect.y = sprite.entity->p.y - camera_position.y;
		rect.w = sprite.rect.w;
		rect.h = sprite.rect.h;
		SDL_RenderCopy(render, sprite.texture, &sprite.rect, &rect);
	}
}
