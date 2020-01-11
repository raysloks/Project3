#include "FollowCursor.h"

#include "Enemy.h"

void FollowCursor::tick(float dt)
{
	entity->p = srs->screenToWorld(input->getCursor());

	Enemy * hover = nullptr;

	auto sprite = getComponent<Sprite>();
	if (sprite)
	{
		sprite->color = SDL_Color();
		auto in_range = cs->overlapCircle(entity->p, 8.0f);
		for (auto i : in_range)
		{
			auto enemy = i.second->getComponent<Enemy>();
			if (enemy)
				hover = enemy;
		}
	}

	if (hover)
	{
		if (current_hover != hover)
		{
			t = 0.0f;
		}
		
		t += dt;

		auto enemy_sprite = hover->getComponent<Sprite>();
		if (enemy_sprite)
		{
			if (current_enemy_sheet != enemy_sprite->sheet)
			{
				current_enemy_sheet = enemy_sprite->sheet;
				sprite->sheet = current_enemy_sheet->makeOutline();
			}

			uint8_t a = cosf(t * 6) * 50 + 200;

			entity->p = enemy_sprite->entity->p;
			sprite->color = SDL_Color({ 255, 0, 0, a });
			sprite->sort = enemy_sprite->sort - 1;
			sprite->flip = enemy_sprite->flip;
			sprite->rotation = enemy_sprite->rotation;
			sprite->subsprite_x = enemy_sprite->subsprite_x;
			sprite->subsprite_y = enemy_sprite->subsprite_y;
		}

		current_hover = hover;
	}
}
