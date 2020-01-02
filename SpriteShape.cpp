#include "SpriteShape.h"

#include "Rectangle.h"

#include <SDL.h>

Rectangle rectangle(Vec2(0.5f, 0.5f));

#include <iostream>

void SpriteShape::check(const Vec2 & diff, const Shape * other, std::vector<Collision>& collisions) const
{
	if (sprite->sheet)
	{
		if (sprite->sheet->surface)
		{
			SDL_Rect src = sprite->sheet->surface->clip_rect;
			src.w /= sprite->sheet->columns;
			src.h /= sprite->sheet->rows;
			src.x += 0 * src.w;
			src.y += 0 * src.h;

			Vec2 center(src.w / 2, src.h / 2);

			for (int x = 0; x < src.w; ++x)
			{
				for (int y = 0; y < src.h; ++y)
				{
					Vec2 rm(x, y);
					rm -= center;

					rm = Vec2(rm.y * sinf(sprite->rotation) - rm.x * cosf(sprite->rotation), rm.x * sinf(sprite->rotation) - rm.y * cosf(sprite->rotation));

					if (sprite->flip & SDL_FLIP_HORIZONTAL)
						rm.x = -rm.x;
					if (sprite->flip & SDL_FLIP_VERTICAL)
						rm.y = -rm.y;

					rm += center;

					auto sample_x = std::llround(rm.x);
					auto sample_y = std::llround(rm.y);

					if (sample_x >= 0 && sample_y >= 0 && sample_x < src.w && sample_y < src.h)
					{
						sample_x += src.x;
						sample_y += src.y;
						SDL_Color * pixel = (SDL_Color*)((uint8_t*)sprite->sheet->surface->pixels + sample_y * sprite->sheet->surface->pitch + sample_x * sizeof(SDL_Color));
						if (pixel->a > 127)
						{
							rectangle.check(diff + Vec2(x, y) - center, other, collisions);
						}
					}
				}
			}
		}
	}
}
