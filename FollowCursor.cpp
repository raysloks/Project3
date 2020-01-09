#include "FollowCursor.h"

#include "Enemy.h"

void FollowCursor::tick(float dt)
{
	entity->p = srs->screenToWorld(input->getCursor());

	auto sprite = getComponent<Sprite>();
	if (sprite)
	{
		sprite->color = SDL_Color({ 100, 100, 200, 50 });
		auto in_range = cs->overlapCircle(entity->p, 6.0f);
		for (auto i : in_range)
		{
			auto enemy = i.second->getComponent<Enemy>();
			if (enemy)
			{
				sprite->color = SDL_Color({ 200, 100, 100, 100 });
			}
		}
	}
}
