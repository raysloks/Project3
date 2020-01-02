#include "SpriteAnimator.h"

SpriteAnimator::SpriteAnimator(float speed, float time, size_t frame_count) : speed(speed), time(time), frame_count(frame_count), destroy(false)
{
}

void SpriteAnimator::tick(float dt)
{
	time += dt * speed;

	if (auto sprite = entity->getComponent<Sprite>())
	{
		size_t frame_count_final = frame_count;
		if (frame_count_final == 0)
			frame_count_final = sprite->sheet->rows * sprite->sheet->columns;
		if (destroy)
			if (time >= frame_count_final)
			{
				engine->remove_entity(entity);
				return;
			}
		time = fmodf(time, frame_count_final);
		size_t frame = time;
		sprite->subsprite_x = frame % sprite->sheet->columns;
		sprite->subsprite_y = frame / sprite->sheet->columns;
	}
}
