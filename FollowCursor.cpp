#include "FollowCursor.h"

#include "Mob.h"

void FollowCursor::tick(float dt)
{
	entity->xy = srs->screenToWorld(input->getCursor());

	Reference<Mob> hover = nullptr;

	auto sprite = getComponent<Sprite>();
	if (sprite)
	{
		sprite->color = SDL_Color();
		auto in_range = cs->overlapCircle(entity->xy + Vec2(4.0f), 8.0f);
		for (auto i : in_range)
		{
			auto mob = i.second->getComponent<Mob>();
			if (mob)
			{
				hover = mob;
				break;
			}
		}
	}

	srs->setCursor(SpriteSheet::get("cursor.png"), 1);

	if (hover)
	{
		if (current_hover != hover)
		{
			t = 0.0f;
		}
		
		t += dt;

		auto hover_sprite = hover->getComponent<Sprite>();
		if (hover_sprite)
		{
			if (current_enemy_sheet != hover_sprite->sheet)
			{
				current_enemy_sheet = hover_sprite->sheet;
				sprite->sheet = current_enemy_sheet->makeOutline();
			}

			uint8_t a = cosf(t * 6) * 50 + 200;

			entity->xyz = hover_sprite->entity->xyz;
			sprite->sort = 256;
			sprite->flip = hover_sprite->flip;
			sprite->rotation = hover_sprite->rotation;
			sprite->subsprite_x = hover_sprite->subsprite_x;
			sprite->subsprite_y = hover_sprite->subsprite_y;

			if (hover->team == 0)
			{
				sprite->color = SDL_Color({ 0, 255, 0, a });
			}
			else
			{
				sprite->color = SDL_Color({ 255, 0, 0, a });
				srs->setCursor(SpriteSheet::get("cursor_attack.png"), 1);
			}
		}

		current_hover = hover;
	}
}
