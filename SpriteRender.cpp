#include "SpriteRender.h"

#include <SDL.h>

SpriteRender::SpriteRender(ComponentContainer<Sprite> * sprites, SDL_Surface * surface)
{
	//this->sprites = sprites;
	this->surface = surface;
}

void SpriteRender::tick(float dt)
{
	for (auto sprite : sprites.components)
	{
		SDL_Rect rect;
		rect.x = sprite.entity->p.x;
		rect.y = sprite.entity->p.y;
		SDL_BlitSurface(sprite.surface, &sprite.rect, surface, &rect);
	}
}
