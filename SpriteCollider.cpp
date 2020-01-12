#include "SpriteCollider.h"

#include "RectangleCollider.h"
#include "CircleCollider.h"

#include <cmath>

RectangleCollider rectangle(0.5f);

SpriteCollider::SpriteCollider(const Reference<Sprite> & sprite) : sprite(sprite), rotation(0.0f)
{
}

void SpriteCollider::check(const Vec2 & diff, const CircleCollider & circle, std::vector<Collision>& collisions) const
{
	auto sheet = sprite->sheet;
	if (non_iso)
		sheet = non_iso;
	if (sheet)
	{
		auto surface = sheet->surface;
		if (sheet->surface)
		{
			SDL_Rect src = surface->clip_rect;
			src.w /= sheet->columns;
			src.h /= sheet->rows;
			src.x += sprite->subsprite_x * src.w;
			src.y += sprite->subsprite_y * src.h;

			Vec2 center(src.w / 2, src.h / 2);
			Vec2 diff_corner = diff + center;

			float rads = rotation * M_PI / 180.0f;
			float sin = sinf(rads);
			float cos = cosf(rads);

			int min_x = -src.h;
			int min_y = -src.w;
			int max_x = src.w + src.h;
			int max_y = src.h + src.w;

			min_x = fmaxf(min_x, diff_corner.x - circle.r);
			min_y = fmaxf(min_y, diff_corner.y - circle.r);
			max_x = fminf(max_x, diff_corner.x + circle.r + 1.0f);
			max_y = fminf(max_y, diff_corner.y + circle.r + 1.0f);

			uint8_t * pixels = (uint8_t*)surface->pixels;
			intmax_t pitch = surface->pitch;

			for (intmax_t x = min_x; x < max_x; ++x)
			{
				for (intmax_t y = min_y; y < max_y; ++y)
				{
					Vec2 rm(x, y);
					rm -= center;

					rm = Vec2(rm.y * sin + rm.x * cos, -rm.x * sin + rm.y * cos);

					if (sprite->flip & SDL_FLIP_HORIZONTAL)
						rm.x = -rm.x;
					if (sprite->flip & SDL_FLIP_VERTICAL)
						rm.y = -rm.y;

					rm += center;

					auto sample_x = llround(rm.x);
					auto sample_y = llround(rm.y);

					if (sample_x >= 0 && sample_y >= 0 && sample_x < src.w && sample_y < src.h)
					{
						sample_x += src.x;
						sample_y += src.y;
						SDL_Color * pixel = (SDL_Color*)(pixels + sample_y * pitch + sample_x * sizeof(SDL_Color));
						if (pixel->a > 127)
						{
							rectangle.check(diff_corner - Vec2(x, y), circle, collisions);
						}
					}
				}
			}
		}
	}
}
