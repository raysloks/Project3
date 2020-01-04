#include "SpriteShape.h"

#include "Rectangle.h"
#include "Circle.h"

#include <SDL.h>

Rectangle rectangle(Vec2(0.5f, 0.5f));

#include <iostream>

SpriteShape::SpriteShape()
{
}

SpriteShape::SpriteShape(Reference<Sprite> sprite) : sprite(sprite)
{
}

void SpriteShape::check(const Vec2 & diff, const Shape * other, std::vector<Collision>& collisions) const
{
	if (sprite->sheet)
	{
		if (sprite->sheet->surface)
		{
			SDL_Rect src = sprite->sheet->surface->clip_rect;
			src.w /= sprite->sheet->columns;
			src.h /= sprite->sheet->rows;
			src.x += sprite->subsprite_x * src.w;
			src.y += sprite->subsprite_y * src.h;

			Vec2 center(src.w / 2, src.h / 2);
			Vec2 diff_corner = diff + center;

			float rads = sprite->rotation * M_PI / 180.0f;
			float sin = sinf(rads);
			float cos = cosf(rads);

			int min_x = -src.h;
			int min_y = -src.w;
			int max_x = src.w + src.h;
			int max_y = src.h + src.w;

			if (auto rectangle = dynamic_cast<const Rectangle*>(other))
			{
				min_x = fmaxf(min_x, diff_corner.x - rectangle->size.x);
				min_y = fmaxf(min_y, diff_corner.y - rectangle->size.y);
				max_x = fminf(max_x, diff_corner.x + rectangle->size.x + 1.0f);
				max_y = fminf(max_y, diff_corner.y + rectangle->size.y + 1.0f);
			}

			if (auto circle = dynamic_cast<const Circle*>(other))
			{
				min_x = fmaxf(min_x, diff_corner.x - circle->r);
				min_y = fmaxf(min_y, diff_corner.y - circle->r);
				max_x = fminf(max_x, diff_corner.x + circle->r + 1.0f);
				max_y = fminf(max_y, diff_corner.y + circle->r + 1.0f);
			}

			for (int x = min_x; x < max_x; ++x)
			{
				for (int y = min_y; y < max_y; ++y)
				{
					Vec2 rm(x, y);
					rm -= center;

					rm = Vec2(rm.y * sin + rm.x * cos, -rm.x * sin + rm.y * cos);

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
							rectangle.check(diff_corner - Vec2(x, y), other, collisions);
						}
					}
				}
			}
		}
	}
}
