#include "SpriteRenderSystem.h"

#include <SDL.h>

SpriteRenderSystem::SpriteRenderSystem(ComponentContainer<Sprite> * sprites, SDL_Renderer * render)
{
	//this->sprites = sprites;
	this->render = render;
}

void SpriteRenderSystem::tick(float dt)
{
	for (auto sprite : sprites.components)
	{
		SDL_Rect rect;
		rect.x = sprite.entity->p.x;
		rect.y = sprite.entity->p.y;
		rect.w = sprite.rect.w;
		rect.h = sprite.rect.h;
		SDL_RenderCopy(render, sprite.texture, &sprite.rect, &rect);
	}
}
