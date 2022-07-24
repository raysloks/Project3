#pragma once

#include "CustomBehaviour.h"

class Mob;

class FollowCursor :
	public CustomBehaviour
{
public:
	void tick(float dt);

	std::shared_ptr<SpriteSheet> current_enemy_sheet;

	Reference<Mob> current_hover;

	float t;

	std::shared_ptr<SpriteSheet> base_cursor_sheet;
};

